#ifndef PEER_IO_THREAD_H_
#define PEER_IO_THREAD_H_

#if WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "common/common_export_macro.h"
#include "common/peer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct PeerIOThread {
#if WIN32
	unsigned int system_thread_id;
#else
	pthread_t system_thread_id;
#endif
	struct event_base *base;
	int thread_id;
	struct Peer *peers;
	struct bufferevent *tunnel[2];
	struct event *timeout_ev;
	long timeout_sec;
};

struct PeerIOThread_AddSocketTask;

typedef int16_t PeerIOThreadTaskType;
typedef void (*peer_init_extra_fn)(struct Peer *, struct PeerIOThread_AddSocketTask *);

struct PeerIOThread_AddSocketTask
{
	PeerIOThreadTaskType type;
	evutil_socket_t fd;
	struct sockaddr_storage ss;
	socklen_t slen;
	int peer_size;
	peer_callback_fn read_cb;
	peer_callback_fn write_cb;
	peer_callback_fn event_cb;
	peer_init_extra_fn init_cb;
};

common_EXPORT struct PeerIOThread* createPeerIOThread(int id, int max_peer_num);

common_EXPORT int peerIOThreadSetTimeout(struct PeerIOThread *io_thread, long timeout_sec, long check_interval_sec);
common_EXPORT int peerIOThreadAddSocket(struct PeerIOThread *io_thread, struct PeerIOThread_AddSocketTask *task);
common_EXPORT int peerIOThreadGetPeerCnt(struct PeerIOThread *io_thread);

#ifdef __cplusplus
}
#endif

#endif