#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include "common/utils.h"
#include "common/peer.h"
#include "common/object.h"
#include "common/peer_io_thread.h"

#if WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

struct CustomPeer
{
	struct Peer base_peer;
	time_t connect_time;
};

struct PeerIOThread** g_io_threads = NULL;
int g_io_thread_cnt = 0;

void myInitPeer(struct Peer *peer, struct PeerIOThread_AddSocketTask *task)
{
	struct CustomPeer *custom_peer = (struct CustomPeer*)peer;
	custom_peer->connect_time = time(NULL);
}

void myReadcb(struct Peer *peer, short events)
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
		if (total_bytes >= len)
		{
			void *stream_bytes = malloc(len);
			if (evbuffer_remove(input_ev, stream_bytes, (size_t)len) < 0)
			{
				return;
			}

			fprintf(stdout, "peer[%ld]:\n", (long)((struct CustomPeer*)peer)->connect_time);
			struct baseObj* obj = decodeMsg(stream_bytes, len);
			if (obj != NULL)
			{
				printObj(obj);
				free(obj);
			}
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
	static io_thread_idx = 0;
	if (g_io_threads)
	{
		struct PeerIOThread_AddSocketTask task;
		memset(&task, 0, sizeof(task));
		task.fd = fd;
		task.ss = *ss;
		task.slen = slen;
		task.peer_size = sizeof(struct CustomPeer);
		task.read_cb = myReadcb;
		task.event_cb = myEventcb;
		task.init_cb = myInitPeer;

		peerIOThreadAddSocket(g_io_threads[io_thread_idx++], &task);
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

void do_accept(evutil_socket_t listener, short event, void *arg)
{
	struct event_base *base = arg;
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

struct PeerIOThread** createIOThread(int cnt)
{
	struct PeerIOThread **io_threads = malloc(sizeof(struct PeerIOThread*) * (cnt+1));
	int i = 0;

	for (i = 0; i < cnt; ++i)
	{
		io_threads[i] = createPeerIOThread(i, 0);
	}
	io_threads[cnt] = NULL;

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

	struct PeerIOThread **p = g_io_threads;
	long sec = 1;
	while (*p)
	{
		peerIOThreadSetTimeout(*p, 15, sec);
		sec += 1;
		++p;
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