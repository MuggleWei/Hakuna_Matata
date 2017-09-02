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

// internal functions
static void peerFree(struct Peer *peer);
static void peerClearTimeTask(struct Peer *peer);
static void peerSetupReconn(struct Peer *peer);

static void peerFree(struct Peer *peer)
{
	if (peer)
	{
		peerSetHeartbeat((struct Peer*)peer, NULL, NULL);
		if (peer->reconn_ev)
		{
			event_free(peer->reconn_ev);
			peer->reconn_ev = NULL;
		}
		if (peer->bev)
		{
			bufferevent_free(peer->bev);
			peer->bev = NULL;
		}

		free(peer);
	}
}
static void peerClearTimeTask(struct Peer *peer)
{
	peerSetHeartbeat(peer, NULL, NULL);
	if (peer->reconn_ev)
	{
		event_free(peer->reconn_ev);
		peer->reconn_ev = NULL;
	}
}
static void peerSetupReconn(struct Peer *peer)
{
	// clear time task
	peerClearTimeTask(peer);

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

	struct timeval time_interval = { peer->reconn_interval, 0 };
	if (event_add(ev, &time_interval) < 0)
	{
		event_free(ev);
		return;
	}

	peer->reconn_ev = ev;
}

static void readcb(struct bufferevent *bev, void *ctx)
{
	struct Peer *peer = (struct Peer*)ctx;
	assert(bev == peer->bev);
	if (peer->func_read)
	{
		(peer->func_read)(peer, 0);
	}

	if (peer->flag & PEER_CLOSE) peerFree(peer);
}
static void writecb(struct bufferevent *bev, void *ctx)
{
	struct Peer *peer = (struct Peer*)ctx;
	assert(bev == peer->bev);
	if (peer->func_write)
	{
		(peer->func_write)(peer, 0);
	}

	if (peer->flag & PEER_CLOSE) peerFree(peer);
}
static void eventcb(struct bufferevent *bev, short events, void *ctx)
{
	struct Peer *peer = (struct Peer*)ctx;
	assert(bev == peer->bev);

	if (events & BEV_EVENT_CONNECTED)
	{
		peerClearTimeTask(peer);
	}

	if (peer->func_event)
	{
		(peer->func_event)(peer, events);
	}

	if ((events & BEV_EVENT_ERROR) || (events & BEV_EVENT_EOF))
	{
		if (peer->flag & PEER_AUTO_RECONNECT)
		{
			peerSetupReconn(peer);
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
	if (peer->func_heartbeat)
	{
		(peer->func_heartbeat)(peer, events);
	}

	if (peer->flag & PEER_CLOSE) peerFree(peer);
}
static void reconncb(evutil_socket_t fd, short events, void *arg)
{
	struct Peer *peer = (struct Peer*)arg;

	bufferevent_socket_connect(peer->bev, (struct sockaddr *)&peer->sock_storage, peer->sockaddr_len);
}


struct Peer* peerConnect(struct event_base *base, const char *addr, int peer_size)
{
	struct sockaddr_storage ss;
	int sockaddr_len = (int)sizeof(ss);
	struct bufferevent *bev = NULL;

	if (evutil_parse_sockaddr_port(addr, (struct sockaddr*)&ss, &sockaddr_len) < 0)
	{
		return NULL;
	}

	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		return NULL;
	}

	if (bufferevent_socket_connect(bev, (struct sockaddr *)&ss, sockaddr_len) < 0)
	{
		bufferevent_free(bev);
		return NULL;
	}

	peer_size = sizeof(struct Peer) > (size_t)peer_size ? sizeof(struct Peer) : peer_size;
	struct Peer *peer = (struct Peer*)malloc(peer_size);
	if (!peer)
	{
		bufferevent_free(bev);
		return NULL;
	}

	bufferevent_setcb(bev, readcb, writecb, eventcb, (void*)peer);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	memset(peer, 0, sizeof(struct Peer));
	peer->bev = bev;
	strncpy(peer->addr, addr, sizeof(peer->addr));
	peer->sock_storage = ss;
	peer->sockaddr_len = sockaddr_len;

	return peer;
}
struct Peer* peerConnectIn(struct event_base *base, evutil_socket_t fd, 
	struct sockaddr_storage *ss, socklen_t slen, int peer_size)
{
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
	
	peer->bev = bev;
	get_sockaddr_port((struct sockaddr*)ss, peer->addr, sizeof(peer->addr));
	peer->sock_storage = *ss;
	peer->sockaddr_len = slen;

	return peer;
}
void peerClose(struct Peer *peer)
{
	peer->flag |= PEER_CLOSE;
}

int peerSetCb(struct Peer *peer, peer_callback_fn readfn, peer_callback_fn writefn, peer_callback_fn eventfn)
{
	if (!peer)
	{
		return -1;
	}
	peer->func_read = readfn;
	peer->func_write = writefn;
	peer->func_event = eventfn;

	return 0;
}
void peerSetAutoReconn(struct Peer *peer, long interval_sec)
{
	if (!peer)
	{
		return;
	}

	if (interval_sec > 0)
	{
		peer->flag |= PEER_AUTO_RECONNECT;
		peer->reconn_interval = interval_sec;
	}
	else
	{
		peer->flag &= ~PEER_AUTO_RECONNECT;
	}
}

int peerSetHeartbeat(struct Peer *peer, peer_callback_fn heartbeatfn, struct timeval *time_interval)
{
	if (!peer)
	{
		return -1;
	}

	if (!peer->bev)
	{
		return -2;
	}

	if (heartbeatfn)
	{
		struct event_base *base = bufferevent_get_base(peer->bev);
		if (!base)
		{
			return -3;
		}

		struct event *ev = event_new(base, -1, EV_PERSIST, heartbeatcb, (void*)peer);
		if (!ev)
		{
			return -4;
		}

		if (event_add(ev, time_interval) < 0)
		{
			event_free(ev);
			return -5;
		}

		if (peer->heartbeat_ev)
		{
			event_free(peer->heartbeat_ev);
		}
		peer->heartbeat_ev = ev;

		peer->flag |= PEER_ENABLE_HEARTBEAT;
	}
	else
	{
		if (peer->heartbeat_ev)
		{
			event_free(peer->heartbeat_ev);
			peer->heartbeat_ev = NULL;
		}
		peer->flag &= ~PEER_ENABLE_HEARTBEAT;
	}

	peer->func_heartbeat = heartbeatfn;
	return 0;
}
