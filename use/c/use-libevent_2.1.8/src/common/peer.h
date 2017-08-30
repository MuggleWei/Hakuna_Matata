#ifndef PEER_H_
#define PEER_H_

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	PEER_ENABLE_HEARTBEAT = 0x01,
	PEER_AUTO_RECONNECT = 0x02,
	PEER_CLOSE = 0x04,
};

typedef void(*peer_callback_fn)(struct Peer *peer, short events);

struct Peer {
	int flag;
	struct bufferevent *bev;
	char addr[128];
	struct sockaddr_storage sock_storage;
	int sockaddr_len;
	peer_callback_fn func_read;
	peer_callback_fn func_write;
	peer_callback_fn func_event;
	peer_callback_fn func_heartbeat;
	struct event *heartbeat_ev;
	struct event *reconn_ev;
	long reconn_interval;
};

// peer connect to address
struct Peer* peerConnect(struct event_base *base, const char *addr, int peer_size);
void peerClose(struct Peer *peer);

// peer callbacks
int peerSetCb(struct Peer *peer, peer_callback_fn readfn, peer_callback_fn writefn, peer_callback_fn eventfn);
void peerSetAutoReconn(struct Peer *peer, long interval_sec);
int peerSetHeartbeat(struct Peer *peer, peer_callback_fn heartbeatfn, struct timeval *time_interval);

#ifdef __cplusplus
}
#endif

#endif