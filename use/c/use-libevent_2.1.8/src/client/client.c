#include "common/peer.h"

#if WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

void myHeartbeat(struct Peer *peer, short events)
{
	char msg[256];
	struct timeval tv;

	if (evutil_gettimeofday(&tv, NULL) == 0)
	{
		snprintf(msg, sizeof(msg), "hello world: %ld.%06ld\n", tv.tv_sec, tv.tv_usec);
		bufferevent_write(peer->bev, msg, strlen(msg));
	}
}

void myReadCb(struct Peer *peer, short events)
{
	char buf[128];
	size_t n;

	while (1)
	{
		n = bufferevent_read(peer->bev, buf, sizeof(buf));
		if (n <= 0)
		{
			break;
		}

		for (int i = 0; i < n; ++i)
		{
			fprintf(stdout, "%c", buf[i]);
		}
	}
}
void myEventCb(struct Peer *peer, short events)
{
	if (events & BEV_EVENT_CONNECTED)
	{
		fprintf(stdout, "connected to %s\n", peer->base_info.remote_addr);

		struct timeval time_interval = { 0,50000 };
		peerSetHeartbeat(peer, myHeartbeat, &time_interval);
	}
	else if (events & BEV_EVENT_ERROR)
	{
		fprintf(stdout, "disconnected %s\n", peer->base_info.remote_addr);
	}
	else if (events & BEV_EVENT_EOF)
	{
		fprintf(stdout, "eof %s\n", peer->base_info.remote_addr);
	}
}


void run()
{
	struct event_base *base = NULL;

	base = event_base_new();
	struct Peer *peer = peerConnectTo(base, "127.0.0.1:40713", 0);
	peerSetCb(peer, myReadCb, NULL, myEventCb);
	peerSetReconn(peer, 1, 0);

	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
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