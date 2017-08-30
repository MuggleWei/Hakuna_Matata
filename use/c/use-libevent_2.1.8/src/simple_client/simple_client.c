#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#if WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

struct event *time_task_ev = NULL;

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

		for (int i = 0; i < n; ++i)
		{
			printf("%c", buf[i]);
		}
	}
}

void cb_func(evutil_socket_t fd, short what, void *arg)
{
	char msg[256];
	struct timeval tv;

	if (evutil_gettimeofday(&tv, NULL) == 0)
	{
		snprintf(msg, sizeof(msg), "hello world: %ld.%06ld\n", tv.tv_sec, tv.tv_usec);
		struct bufferevent *bev = (struct bufferevent*)arg;
		bufferevent_write(bev, msg, strlen(msg));
	}
}

void eventcb(struct bufferevent *bev, short events, void *ctx)
{
	if (events & BEV_EVENT_CONNECTED)
	{
		printf("connected\n");

		struct timeval time_interval = { 0,500000 };

		struct event_base *base = bufferevent_get_base(bev);
		int ret = 0;

		time_task_ev = event_new(base, -1, EV_PERSIST, cb_func, (void*)bev);
		if (!time_task_ev)
		{
			fprintf(stderr, "failed create time task event\n");
			return;	
		}

		ret = event_add(time_task_ev, &time_interval);
		if (ret < 0)
		{
			fprintf(stderr, "failed event_add time_task_ev\n");
			return;
		}
	}
	else if (events & BEV_EVENT_EOF)
	{
		printf("event eof\n");
	}
	else if (events & BEV_EVENT_ERROR)
	{
		printf("event error\n");

		if (time_task_ev)
		{
			event_free(time_task_ev);
		}
		bufferevent_free(bev);
	}
	else if (events & BEV_EVENT_TIMEOUT)
	{
		printf("event timeout\n");
	}
}

void run()
{
	struct event_base *base = NULL;
	struct bufferevent *bev = NULL;
	struct sockaddr_storage ss;
	int sockaddr_len = (int)sizeof(ss);

	memset(&ss, 0, sizeof(ss));

	base = event_base_new();

	if (evutil_parse_sockaddr_port("127.0.0.1:40713", (struct sockaddr*)&ss, &sockaddr_len) < 0)
	{
		return;
	}

	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		return;
	}

	bufferevent_setcb(bev, readcb, NULL, eventcb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	if (bufferevent_socket_connect(bev, (struct sockaddr *)&ss, sockaddr_len) < 0)
	{
		bufferevent_free(bev);
		return;
	}

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
