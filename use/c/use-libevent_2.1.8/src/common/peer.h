#ifndef PEER_H_
#define PEER_H_

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "common/common_export_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	PEER_AUTO_RECONNECT = 0x01,
	PEER_WAIT_CLOSE = 0x02,
};

struct Peer;

typedef void (*peer_callback_fn)(struct Peer *peer, short events);
typedef void (*peer_close_callback_fn)(struct Peer *peer);

struct PeerBaseInfo
{
	char local_addr[128];
	char remote_addr[128];
	struct sockaddr_storage remote_ss;
	int remote_ss_len;
	struct timeval first_connect_time;
	void *thread;
};

struct PeerCallback
{
	peer_callback_fn func_read;
	peer_callback_fn func_write;
	peer_callback_fn func_event;
	peer_callback_fn func_heartbeat;
	peer_close_callback_fn func_close;
};

struct PeerTimeout
{
	time_t last_read;
	time_t last_write;
	unsigned short in_timeout;
	unsigned short out_timeout;
};

struct PeerHeartbeat
{
	struct event *ev;
	struct timeval interval;
};

struct PeerReconn
{
	struct event *ev;
	unsigned short interval;
	unsigned short max_increase;
};

struct PeerBaseEvent
{
	struct PeerHeartbeat hearbeat;
	struct PeerTimeout timeout;
	struct PeerReconn reconn;
};

/*
 *	NOTE: 
 *		in reconn and heartbeat, create ev is not thread safe, but
 *		I only invoke it in init or close it in the same thread, so
 *		it's ok.
 */
struct Peer
{
	int flag;
	struct bufferevent *bev;
	struct PeerBaseInfo base_info;
	struct PeerCallback callbacks;
	struct PeerBaseEvent events;
};

/*
 *	peerConnectTo: connect to addr
 *	@addr: the address of target, format ip:port
 *	@peer_size: the size of peer, the real space size of return is: peer_size > sizeof(struct Peer) ? peer_size : sizeof(struct Peer)
 */
common_EXPORT struct Peer* peerConnectTo(struct event_base *base, const char *addr, int peer_size);

/*
 *	peerConnectIn: when new socket accept
 */
common_EXPORT struct Peer* peerConnectIn(struct event_base *base, evutil_socket_t fd, 
	struct sockaddr_storage *ss, socklen_t slen, int peer_size);

// set close flag, it will not free immediately, wait for the event processing is completed
common_EXPORT void peerClose(struct Peer *peer);

// free peer immediately
common_EXPORT void peerFree(struct Peer *peer);

common_EXPORT void peerSetCb(struct Peer *peer, peer_callback_fn readfn, peer_callback_fn writefn, peer_callback_fn eventfn);
common_EXPORT void peerSetReadCb(struct Peer *peer, peer_callback_fn fn);
common_EXPORT void peerSetWriteCb(struct Peer *peer, peer_callback_fn fn);
common_EXPORT void peerSetEventCb(struct Peer *peer, peer_callback_fn fn);
common_EXPORT void peerSetCloseCb(struct Peer *peer, peer_close_callback_fn fn);

/*
 *	peerSetTimeout: timeout of peer
 *	@in_timeout: if it = 0, represent don't disconnect proactively, otherwise represent timeout without incoming traffic
 *	@out_timeout: if it = 0, represent don't disconnect proactively, otherwise represent timeout without outcoming traffic
 */
common_EXPORT int peerSetTimeout(struct Peer *peer, unsigned short in_timeout, unsigned short out_timeout);

/*
 *	peerSetHeartbeat: the interval of send heartbeat proactively
 *	@heartbeatfn: if it's NULL, represent don't send heartbeat
 */
common_EXPORT int peerSetHeartbeat(struct Peer *peer, peer_callback_fn heartbeatfn, struct timeval *time_interval);

/*
 *	peerSetReconn: set reconnected interval time when disconnect happend
 *	@interval_sec: if it > 0, represent reconnect interval time, if it = 0, represent don't reconnect when disconnect
 *	@max_incre: 0 represent don't increment, otherwise max increment multiple
 */
common_EXPORT void peerSetReconn(struct Peer *peer, unsigned short interval_sec, unsigned short max_incre);


#ifdef __cplusplus
}
#endif

#endif
