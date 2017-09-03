#include "peer.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

// internal functions -- default callbacks
static void readcb(struct bufferevent *bev, void *ctx);
static void writecb(struct bufferevent *bev, void *ctx);
static void eventcb(struct bufferevent *bev, short events, void *ctx);
static void heartbeatcb(evutil_socket_t fd, short events, void *arg);
static void reconncb(evutil_socket_t fd, short events, void *arg);
static void peerStartReconn(struct Peer *peer);
static void peerStopReconn(struct Peer *peer);

static void peerStartReconn(struct Peer *peer)
{
	// close fd
	evutil_socket_t fd = bufferevent_getfd(peer->bev);
	if (fd > 0)
	{
		evutil_closesocket(fd);
	}
	bufferevent_setfd(peer->bev, -1);

	bufferevent_setcb(peer->bev, readcb, writecb, eventcb, (void*)peer);
	bufferevent_enable(peer->bev, EV_READ | EV_WRITE);

	// start time task for reconnect
	struct event_base *base = bufferevent_get_base(peer->bev);
	if (!base)
	{
		return;
	}

	struct event *ev = event_new(base, -1, 0, reconncb, (void*)peer);
	if (!ev)
	{
		return;
	}

	struct timeval time_interval = { peer->events.reconn.interval, 0 };
	if (event_add(ev, &time_interval) < 0)
	{
		event_free(ev);
		return;
	}

	peer->events.reconn.ev = ev;
}
static void peerStopReconn(struct Peer *peer)
{
	if (peer->events.reconn.ev)
	{
		event_free(peer->events.reconn.ev);
		peer->events.reconn.ev = NULL;
	}
}

static void readcb(struct bufferevent *bev, void *ctx)
{
	struct Peer *peer = (struct Peer*)ctx;
	assert(bev == peer->bev);
	if (peer->callbacks.func_read)
	{
		(peer->callbacks.func_read)(peer, 0);
	}

	struct timeval t;
	event_base_gettimeofday_cached(bufferevent_get_base(bev), &t);
	peer->events.timeout.last_read = t.tv_sec;

	if (peer->flag & PEER_WAIT_CLOSE) peerFree(peer);
}
static void writecb(struct bufferevent *bev, void *ctx)
{
	struct Peer *peer = (struct Peer*)ctx;
	assert(bev == peer->bev);
	if (peer->callbacks.func_write)
	{
		(peer->callbacks.func_write)(peer, 0);
	}

	struct timeval t;
	event_base_gettimeofday_cached(bufferevent_get_base(bev), &t);
	peer->events.timeout.last_write = t.tv_sec;

	if (peer->flag & PEER_WAIT_CLOSE) peerFree(peer);
}
static void eventcb(struct bufferevent *bev, short events, void *ctx)
{
	struct Peer *peer = (struct Peer*)ctx;
	assert(bev == peer->bev);

	if (events & BEV_EVENT_CONNECTED)
	{
		peerStopReconn(peer);
	}

	if (peer->callbacks.func_event)
	{
		(peer->callbacks.func_event)(peer, events);
	}

	if ((events & BEV_EVENT_ERROR) || (events & BEV_EVENT_EOF))
	{
		if (peer->flag & PEER_AUTO_RECONNECT)
		{
			peerStartReconn(peer);
		}
		else
		{
			peerFree(peer);
		}
	}
}
static void heartbeatcb(evutil_socket_t fd, short events, void *arg)
{
	struct Peer *peer = (struct Peer*)arg;
	if (peer->callbacks.func_heartbeat)
	{
		(peer->callbacks.func_heartbeat)(peer, events);
	}

	if (peer->flag & PEER_WAIT_CLOSE) peerFree(peer);
}
static void reconncb(evutil_socket_t fd, short events, void *arg)
{
	struct Peer *peer = (struct Peer*)arg;

	bufferevent_socket_connect(peer->bev, (struct sockaddr *)&peer->base_info.remote_ss, peer->base_info.remote_ss_len);
}


struct Peer* peerConnectTo(struct event_base *base, const char *addr, int peer_size)
{
	struct sockaddr_storage remote_ss, local_ss;
	int remote_ss_len = (int)sizeof(remote_ss);
	int local_ss_len = sizeof(local_ss);
	struct bufferevent *bev = NULL;

	if (evutil_parse_sockaddr_port(addr, (struct sockaddr*)&remote_ss, &remote_ss_len) < 0)
	{
		return NULL;
	}

	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		return NULL;
	}

	if (bufferevent_socket_connect(bev, (struct sockaddr *)&remote_ss, remote_ss_len) < 0)
	{
		fprintf(stderr, "failed connect to %s\n", addr);
		bufferevent_free(bev);
		return NULL;
	}

	peer_size = sizeof(struct Peer) > (size_t)peer_size ? sizeof(struct Peer) : peer_size;
	struct Peer *peer = (struct Peer*)malloc(peer_size);
	if (!peer)
	{
		fprintf(stderr, "failed allocate peer\n");
		bufferevent_free(bev);
		return NULL;
	}

	bufferevent_setcb(bev, readcb, writecb, eventcb, (void*)peer);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	// fill out base info
	memset(peer, 0, sizeof(struct Peer));
	peer->bev = bev;
	getsockname(bufferevent_getfd(bev), (struct sockaddr*)&local_ss, &local_ss_len);
	get_sockaddr_port((struct sockaddr*)&local_ss, peer->base_info.local_addr, sizeof(peer->base_info.local_addr));
	strncpy(peer->base_info.remote_addr, addr, sizeof(peer->base_info.remote_addr)-1);
	peer->base_info.remote_ss = remote_ss;
	peer->base_info.remote_ss_len = remote_ss_len;
	evutil_gettimeofday(&peer->base_info.first_connect_time, NULL);

	return peer;
}
struct Peer* peerConnectIn(struct event_base *base, evutil_socket_t fd, 
	struct sockaddr_storage *ss, socklen_t slen, int peer_size)
{
	struct sockaddr_storage local_ss;
	int local_ss_len = sizeof(local_ss);
	struct bufferevent *bev;

	peer_size = sizeof(struct Peer) > (size_t)peer_size ? sizeof(struct Peer) : peer_size;
	struct Peer *peer = (struct Peer*)malloc(peer_size);
	if (!peer)
	{
		return NULL;
	}
	memset(peer, 0, sizeof(struct Peer));

	evutil_make_socket_nonblocking(fd);
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (bev == NULL)
	{
		bufferevent_free(bev);
		return NULL;
	}
	bufferevent_setcb(bev, readcb, writecb, eventcb, (void*)peer);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	
	// fill out base info
	peer->bev = bev;
	getsockname(fd, (struct sockaddr*)&local_ss, &local_ss_len);
	get_sockaddr_port((struct sockaddr*)&local_ss, peer->base_info.local_addr, sizeof(peer->base_info.local_addr));
	get_sockaddr_port((struct sockaddr*)ss, peer->base_info.remote_addr, sizeof(peer->base_info.remote_addr));
	peer->base_info.remote_ss = *ss;
	peer->base_info.remote_ss_len = slen;
	evutil_gettimeofday(&peer->base_info.first_connect_time, NULL);

	return peer;
}
void peerClose(struct Peer *peer)
{
	peer->flag |= PEER_WAIT_CLOSE;
}
void peerFree(struct Peer *peer)
{
	if (peer->callbacks.func_close)
	{
		peer->callbacks.func_close(peer);
	}

	peerSetTimeout(peer, 0, 0);
	peerSetHeartbeat(peer, NULL, NULL);
	peerSetReconn(peer, 0, 0);

	if (peer->bev)
	{
		bufferevent_free(peer->bev);
		peer->bev = NULL;
	}

	free(peer);
}

void peerSetCb(struct Peer *peer, peer_callback_fn readfn, peer_callback_fn writefn, peer_callback_fn eventfn)
{
	peerSetReadCb(peer, readfn);
	peerSetWriteCb(peer, writefn);
	peerSetEventCb(peer, eventfn);
}
void peerSetReadCb(struct Peer *peer, peer_callback_fn fn)
{
	peer->callbacks.func_read = fn;
}
void peerSetWriteCb(struct Peer *peer, peer_callback_fn fn)
{
	peer->callbacks.func_write = fn;
}
void peerSetEventCb(struct Peer *peer, peer_callback_fn fn)
{
	peer->callbacks.func_event = fn;
}
void peerSetCloseCb(struct Peer *peer, peer_close_callback_fn fn)
{
	peer->callbacks.func_close = fn;
}

int peerSetTimeout(struct Peer *peer, unsigned short in_timeout, unsigned short out_timeout)
{
	peer->events.timeout.in_timeout = in_timeout;
	peer->events.timeout.out_timeout = out_timeout;
	return 0;
}

int peerSetHeartbeat(struct Peer *peer, peer_callback_fn heartbeatfn, struct timeval *time_interval)
{
	if (!peer->bev)
	{
		return -1;
	}

	if (peer->events.hearbeat.ev)
	{
		event_free(peer->events.hearbeat.ev);
		peer->events.hearbeat.ev = NULL;
	}

	if (heartbeatfn &&
		(time_interval->tv_sec > 0 || time_interval->tv_usec > 0))
	{
		struct event_base *base = bufferevent_get_base(peer->bev);
		if (!base)
		{
			return -2;
		}

		struct event *ev = event_new(base, -1, EV_PERSIST, heartbeatcb, (void*)peer);
		if (!ev)
		{
			return -3;
		}

		if (event_add(ev, time_interval) < 0)
		{
			event_free(ev);
			return -4;
		}

		peer->events.hearbeat.ev = ev;
		peer->events.hearbeat.interval = *time_interval;
		peer->callbacks.func_heartbeat = heartbeatfn;
	}
	else
	{
		peer->callbacks.func_heartbeat = NULL;
	}

	return 0;
}

void peerSetReconn(struct Peer *peer, unsigned short interval_sec, unsigned short max_incre)
{
	if (interval_sec > 0)
	{
		peer->flag |= PEER_AUTO_RECONNECT;
	}
	else
	{
		peer->flag &= ~PEER_AUTO_RECONNECT;

		if (peer->events.reconn.ev)
		{
			event_free(peer->events.reconn.ev);
			peer->events.reconn.ev = NULL;
		}
	}

	peer->events.reconn.interval = interval_sec;
	peer->events.reconn.max_increase = max_incre;
}

