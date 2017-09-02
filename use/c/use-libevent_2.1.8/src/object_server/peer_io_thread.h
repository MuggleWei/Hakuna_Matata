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
#include <set>
#include "common/common_export_macro.h"
#include "common/peer.h"

struct PeerIOThread 
{
	PeerIOThread()
		: system_thread_id(0)
		, thread_id(-1)
		, base(nullptr)
		, tunnel{ nullptr }
		, timeout_ev(nullptr)
		, timeout_sec(0)
	{}

#if WIN32
	unsigned int system_thread_id;
#else
	pthread_t system_thread_id;
#endif
	int thread_id;
	struct event_base *base;
	struct bufferevent *tunnel[2];
	std::set<struct Peer*> peers;
	struct event *timeout_ev;
	unsigned short timeout_sec;
};

struct PeerIOThread_AddSocket;
typedef void (*peer_init_extra_fn)(struct Peer *, struct PeerIOThread_AddSocket *);

typedef unsigned char PeerIOThreadTaskType;
struct PeerIOThread_CheckTimeout
{
	PeerIOThreadTaskType type;
	struct timeval check_time_interval;
};
struct PeerIOThread_AddSocket
{
	PeerIOThreadTaskType type;
	evutil_socket_t fd;
	struct sockaddr_storage remote_ss;
	socklen_t remote_ss_len;
	peer_callback_fn read_cb;
	peer_callback_fn write_cb;
	peer_callback_fn event_cb;
	peer_init_extra_fn init_cb;
};

PeerIOThread* createPeerIOThread(int id);

int peerIOThreadSetTimeout(PeerIOThread *io_thread, unsigned short timeout_sec, long check_interval_sec);
int peerIOThreadAddSocket(PeerIOThread *io_thread, struct PeerIOThread_AddSocket *task);
int peerIOThreadGetPeerCnt(PeerIOThread *io_thread);

#endif