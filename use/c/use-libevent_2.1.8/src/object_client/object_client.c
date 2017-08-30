#if WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
#include <stdio.h>
#include <event2/thread.h>
#include "common/object.h"
#include "common/peer.h"

struct CustomPeer
{
	struct Peer base_peer;
	char connect_name[16];
};

struct Peer *g_peer = NULL;

void testEncodeDecode(struct baseObj *obj)
{
	int len;
	void *stream_bytes = encodeMsg(obj, &len);
	if (stream_bytes)
	{
		struct baseObj* obj = decodeMsg(stream_bytes, len);
		if (obj != NULL)
		{
			printObj(obj);
			free(obj);
		}
		free(stream_bytes);
	}
}

void testObj()
{
	struct Foo foo;
	foo.obj.type = OBJ_TYPE_FOO;
	foo.foo_id = 1;
	strncpy(foo.buf, "hello world", sizeof(foo.buf));
	testEncodeDecode((struct baseObj*)&foo);

	struct Bar bar;
	bar.obj.type = OBJ_TYPE_BAR;
	bar.bar_id = 2;
	for (int i = 0; i < sizeof(bar.buf); ++i)
	{
		bar.buf[i] = 'x';
	}
	bar.buf[sizeof(bar.buf) - 1] = '\0';
	testEncodeDecode((struct baseObj*)&bar);
}

void myHeartbeat(struct Peer *peer, short events)
{
	static int req_id = 0;
	int len;
	struct Foo foo;

	foo.obj.type = OBJ_TYPE_FOO;
	foo.foo_id = ++req_id;
	strncpy(foo.buf, "Hey, it's heartbeat message", sizeof(foo.buf));
	void *stream_bytes = encodeMsg((struct baseObj*)&foo, &len);

	if (stream_bytes)
	{
		bufferevent_write(peer->bev, stream_bytes, len);
		free(stream_bytes);
	}
}

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

			fprintf(stdout, "message from: %s\n", ((struct CustomPeer*)peer)->connect_name);
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
		struct timeval time_interval = { 1, 0 };
		peerSetHeartbeat(peer, myHeartbeat, &time_interval);
	}
	else if (events & BEV_EVENT_ERROR)
	{}
	else if (events & BEV_EVENT_EOF)
	{}
}

#if WIN32
unsigned int threadFunc(void *arg)
#else
void* threadFunc(void *arg)
#endif
{
	struct event_base *base = NULL;

	base = event_base_new();
	g_peer = peerConnect(base, "127.0.0.1:40713", sizeof(struct CustomPeer));
	peerSetCb(g_peer, myReadCb, NULL, myEventCb);
	peerSetAutoReconn(g_peer, 1);

	struct CustomPeer *peer = (struct CustomPeer*)g_peer;
	strncpy(peer->connect_name, "my server", sizeof(peer->connect_name));

	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);

#if WIN32
	return 0;
#else
	return NULL;
#endif
}

void runManual()
{
	while (1)
	{
		getchar();

		if (g_peer)
		{
			static int req_id = 0;
			int len;
			struct Bar bar;

			bar.obj.type = OBJ_TYPE_BAR;
			bar.bar_id = ++req_id;
			strncpy(bar.buf, "Hey, it's manual message", sizeof(bar.buf));
			void *stream_bytes = encodeMsg((struct baseObj*)&bar, &len);

			if (stream_bytes)
			{
				bufferevent_write(g_peer->bev, stream_bytes, len);
				free(stream_bytes);
			}
		}
	}
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

	unsigned int thread_id;
	_beginthreadex(NULL, 0, &threadFunc, NULL, 0, &thread_id);
#else
	if (evthread_use_pthreads() != 0)
	{
		exit(EXIT_FAILURE);
	}

	pthread_t thread_id;
	pthread_create(&th, NULL, threadFunc, NULL);
	pthread_detach(thread_id);
#endif

	runManual();

#if WIN32
	WSACleanup();
#endif

	return 0;
}