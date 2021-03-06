#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "common/utils.h"

#if WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

void readcb(struct bufferevent *bev, void *ctx)
{
	char buf[128];
	size_t n;

	while (1)
	{
		n = bufferevent_read(bev, buf, sizeof(buf));
		if (n <= 0)
		{
			break;
		}
		bufferevent_write(bev, buf, n);
	}
}

void errorcb(struct bufferevent *bev, short error, void *ctx)
{
	if (error & BEV_EVENT_EOF)
	{
		printf("event eof\n");
	}
	else if (error & BEV_EVENT_ERROR)
	{
		printf("event error\n");
	}
	else if (error & BEV_EVENT_TIMEOUT)
	{
		printf("event timeout\n");
	}

	bufferevent_free(bev);
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
		struct bufferevent *bev;

		evutil_make_socket_nonblocking(fd);
		bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
		bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
		bufferevent_setwatermark(bev, EV_READ, 0, 1024 * 1024 * 2);
		bufferevent_enable(bev, EV_READ | EV_WRITE);

		char buf[128];
		get_sockaddr_port((struct sockaddr*)&ss, buf, sizeof(buf));

		fprintf(stdout, "client connected: %s\n", buf);
	}
}

void run(void)
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