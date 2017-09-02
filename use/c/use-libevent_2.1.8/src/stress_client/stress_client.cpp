#if WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/thread.h>
#include <set>
#include <thread>
#include <chrono>
#include "common/object.h"
#include "common/peer.h"
#include "common/utils.h"

std::set<struct Peer*> peers;

const static char *server = "127.0.0.1:40713";

void myReadCb(struct Peer *peer, short events)
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
void myEventCb(struct Peer *peer, short events)
{
	if (events & BEV_EVENT_CONNECTED)
	{
	}
	else if ((events & BEV_EVENT_ERROR) || events & BEV_EVENT_EOF)
	{
		peerClose(peer);
		peers.erase(peer);
	}
}

void send(struct Peer *peer)
{
	int len;
	struct Bar bar;

	bar.obj.type = OBJ_TYPE_BAR;
	bar.bar_id = 0;
	strncpy(bar.buf, "Hey, it's manual message", sizeof(bar.buf));
	void *stream_bytes = encodeMsg((struct baseObj*)&bar, &len);

	if (stream_bytes)
	{
		bufferevent_write(peer->bev, stream_bytes, len);
		free(stream_bytes);
	}
}

void TimeTask(evutil_socket_t fd, short events, void *arg)
{
	for (auto it = peers.begin(); it != peers.end(); ++it)
	{
		if (rand() % 10 > 7)
		{
			send(*it);
		}
	}

	if (rand() % 100 > 2)
	{
		if (peers.size() < 500)
		{
			auto peer = peerConnectTo((struct event_base*)arg, server, 0);
			if (peer)
			{
				peerSetCb(peer, myReadCb, NULL, myEventCb);
				peers.insert(peer);
			}
		}
	}
	else
	{
		if (peers.size() > 0)
		{
			peerFree(*(peers.begin()));
			peers.erase(peers.begin());
		}
	}
	
}

void run()
{
	srand(time(nullptr));

	struct event_base *base = event_base_new();

	for (int i = 0; i < 100; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		auto peer = peerConnectTo(base, server, 0);
		if (peer)
		{
			peerSetCb(peer, myReadCb, NULL, myEventCb);
			peers.insert(peer);
		}
	}

	struct event *ev = event_new(base, -1, EV_PERSIST, TimeTask, (void*)base);
	if (!ev)
	{
		return;
	}

	struct timeval tv = { 0, 200 };
	if (event_add(ev, &tv) < 0)
	{
		event_free(ev);
		return;
	}

	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
}

int main(int argc, char *argv)
{

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

	// for (int i = 0; i < 4; ++i)
	// {
	// 	std::thread th(run);
	// 	th.detach();
	// }

	run();

#if WIN32
	WSACleanup();
#endif

	return 0;
}
