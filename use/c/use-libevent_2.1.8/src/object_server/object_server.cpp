#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "common/utils.h"
#include "common/peer.h"
#include "common/object.h"
#include "peer_io_thread.h"

#if WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

PeerIOThread** g_io_threads = NULL;
int g_io_thread_cnt = 0;

void myInitPeer(struct Peer *peer, struct PeerIOThread_AddSocket * /*task*/)
{
	PeerIOThread *io_thread = (PeerIOThread*)peer->base_info.thread;
	fprintf(stdout, "#%d Thread has peer: %d\n", io_thread->thread_id, (int)io_thread->peers.size());
}

void myReadcb(struct Peer *peer, short /*events*/)
{
	unsigned char *mem;

	struct evbuffer *input_ev = bufferevent_get_input(peer->bev);
	while (1)
	{
		size_t total_bytes = evbuffer_get_length(input_ev);
		if (total_bytes < 4)
		{
			return;
		}

		mem = evbuffer_pullup(input_ev, (ev_ssize_t)4);
		if (mem == NULL)
		{
			return;
		}

		int32_t len = util_ntoh_32(*(int32_t*)mem);
		if ((int32_t)total_bytes >= len)
		{
			void *stream_bytes = malloc(len);
			if (evbuffer_remove(input_ev, stream_bytes, (size_t)len) < 0)
			{
				return;
			}

			struct baseObj* obj = decodeMsg(stream_bytes, len);
			if (obj != NULL)
			{
				// printObj(obj);
				free(obj);
			}
			bufferevent_write(peer->bev, stream_bytes, len);
			free(stream_bytes);
		}
		else
		{
			return;
		}
	}
}

void myEventcb(struct Peer *peer, short events)
{
	if (events & BEV_EVENT_EOF)
	{
		printf("event eof\n");
	}
	else if (events & BEV_EVENT_ERROR)
	{
		printf("event error\n");
	}
	else if (events & BEV_EVENT_TIMEOUT)
	{
		printf("event timeout\n");
	}

	peerClose(peer);
}

void addNewSocket(evutil_socket_t fd, struct sockaddr_storage *ss, socklen_t slen)
{
	static int io_thread_idx = 0;
	if (g_io_threads)
	{
		struct PeerIOThread_AddSocket task;
		memset(&task, 0, sizeof(task));
		task.fd = fd;
		task.remote_ss = *ss;
		task.remote_ss_len = slen;
		task.read_cb = myReadcb;
		task.event_cb = myEventcb;
		task.init_cb = myInitPeer;

		int ret = peerIOThreadAddSocket(g_io_threads[io_thread_idx++], &task);
		if (ret < 0)
		{
			fprintf(stderr, "failed add new socket: %d\n", fd);
			evutil_closesocket(fd);
		}
		if (io_thread_idx >= g_io_thread_cnt)
		{
			io_thread_idx = 0;
		}
	}
	else
	{
		evutil_closesocket(fd);
	}
}

void do_accept(evutil_socket_t listener, short /*event*/, void *arg)
{
	struct sockaddr_storage ss;
	socklen_t slen = sizeof(ss);
	evutil_socket_t fd = accept(listener, (struct sockaddr*)&ss, &slen);
	if (fd < 0)
	{
		perror("accept");
	}
	else
	{
		addNewSocket(fd, &ss, slen);
	}
}

PeerIOThread** createIOThread(int cnt)
{
	PeerIOThread **io_threads = (PeerIOThread**)malloc(sizeof(PeerIOThread*) * cnt);
	int i = 0;

	for (i = 0; i < cnt; ++i)
	{
		io_threads[i] = createPeerIOThread(i);
	}

	return io_threads;
}

void mainloop()
{
	evutil_socket_t listener;
	struct sockaddr_in sin;
	struct event_base *base = NULL;
	struct event *listener_event = NULL;

	memset(&sin, 0, sizeof(sin));

	base = event_base_new();
	if (!base)
	{
		fprintf(stderr, "Failed in event_base_new()\n");
		return;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(40713);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	evutil_make_socket_nonblocking(listener);

#ifndef WIN32
	{
		int one = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
#endif

	if (bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		perror("bind");
		return;
	}

	if (listen(listener, 16) < 0)
	{
		perror("listen");
		return;
	}

	listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)base);
	event_add(listener_event, NULL);

	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
}

void run(void)
{
	// libevent use thread
#if WIN32
	if (evthread_use_windows_threads() != 0)
	{
		exit(EXIT_FAILURE);
	}
#else
	if (evthread_use_pthreads() != 0)
	{
		exit(EXIT_FAILURE);
	}
#endif

	g_io_threads = createIOThread(8);
	g_io_thread_cnt = 8;

	for (int i = 0; i < g_io_thread_cnt; ++i)
	{
		peerIOThreadSetTimeout(g_io_threads[i], 30, 15);
	}

	mainloop();
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

#if WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}
#endif

	run();

#if WIN32
	WSACleanup();
#endif

	return 0;
}
