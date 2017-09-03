#include "peer_io_thread.h"
#include <stdlib.h>
#include <event2/thread.h>

enum eTunnelType
{
	TUNNEL_IN = 0,
	TUNNEL_OUT = 1
};

enum eTunnelInputType
{
	TUNNEL_ADD_TIME = 0,
	TUNNEL_ADD_SOCKET = 1,
};

struct PeerIOThreadArg
{
	volatile PeerIOThread *p_io_thread;
	int id;
#if WIN32
	CRITICAL_SECTION mtx;
	CONDITION_VARIABLE cv;
#else
	pthread_mutex_t mtx;
	pthread_cond_t cv;
#endif
};

// internal functions
static void onClosePeer(struct Peer *peer);
static void check_timeout(evutil_socket_t fd, short events, void *arg);
static void tunnelcb(struct bufferevent *bev, void *ctx);
static int tunnelcb_timetask(struct evbuffer *input, PeerIOThread *io_thread);
static int tunnelcb_addsocktask(struct evbuffer *input, PeerIOThread *io_thread);

void onClosePeer(struct Peer *peer)
{
	PeerIOThread *io_thread = (PeerIOThread*)peer->base_info.thread;
	io_thread->peers.erase(peer);
}
static void check_timeout(evutil_socket_t fd, short /*events*/, void *arg)
{
	PeerIOThread *io_thread = (PeerIOThread*)arg;
	struct timeval t;
	event_base_gettimeofday_cached(io_thread->base, &t);
	time_t cur_time = t.tv_sec;

	std::set<struct Peer*> del_peers;
	for (auto it = io_thread->peers.begin(); it != io_thread->peers.end(); ++it)
	{
		if (((*it)->events.timeout.in_timeout != 0 && cur_time - (*it)->events.timeout.last_read > (*it)->events.timeout.in_timeout) ||
			((*it)->events.timeout.out_timeout != 0 && cur_time - (*it)->events.timeout.last_write > (*it)->events.timeout.out_timeout))
		{
			fprintf(stdout, "#%d thread: %s is timeout", io_thread->thread_id, (*it)->base_info.remote_addr);
			del_peers.insert(*it);
			// Don't do that, cause I erase in onClosePeer
			// it = io_thread->peers.erase(it);
		}
	}

	for (auto it = del_peers.begin(); it != del_peers.end(); ++it)
	{
		peerFree(*it);
	}
}
static void tunnelcb(struct bufferevent *bev, void *ctx)
{
	PeerIOThread *io_thread = (PeerIOThread*)ctx;
	struct evbuffer *input = bufferevent_get_input(bev);
	while (evbuffer_get_length(input) >= sizeof(PeerIOThreadTaskType))
	{
		PeerIOThreadTaskType task_type;
		if (evbuffer_copyout(input, &task_type, sizeof(PeerIOThreadTaskType)) < 0)
		{
			return;
		}

		int ret = 0;
		switch (task_type)
		{
		case TUNNEL_ADD_TIME:
		{
			ret = tunnelcb_timetask(input, io_thread);
		}break;
		case TUNNEL_ADD_SOCKET:
		{
			ret = tunnelcb_addsocktask(input, io_thread);
		}break;
		}

		if (ret != 0)
		{
			return;
		}
	}
}
static int tunnelcb_timetask(struct evbuffer *input, PeerIOThread *io_thread)
{
	struct PeerIOThread_CheckTimeout task;
	if (evbuffer_get_length(input) >= sizeof(task))
	{
		evbuffer_remove(input, &task, sizeof(task));
	}
	else
	{
		return -1;
	}

	if (io_thread->timeout_ev)
	{
		event_free(io_thread->timeout_ev);
		io_thread->timeout_ev = NULL;
	}

	if (task.check_time_interval.tv_sec > 0 || task.check_time_interval.tv_usec > 0)
	{
		struct event *ev = event_new(io_thread->base, -1, EV_PERSIST, check_timeout, (void*)io_thread);
		if (!ev)
		{
			return -2;
		}

		if (event_add(ev, &task.check_time_interval) < 0)
		{
			event_free(ev);
			return -3;
		}

		io_thread->timeout_ev = ev;
	}

	return 0;
}
static int tunnelcb_addsocktask(struct evbuffer *input, PeerIOThread *io_thread)
{
	struct PeerIOThread_AddSocket task;
	if (evbuffer_get_length(input) >= sizeof(task))
	{
		evbuffer_remove(input, &task, sizeof(task));
		struct Peer *peer = peerConnectIn(io_thread->base, task.fd, &task.remote_ss, task.remote_ss_len, 0);
		if (!peer)
		{
			return -1;
		}

		struct timeval t;
		event_base_gettimeofday_cached(io_thread->base, &t);

		peerSetCb(peer, task.read_cb, task.write_cb, task.event_cb);
		peerSetCloseCb(peer, onClosePeer);
		peer->base_info.thread = (void*)io_thread;
		peer->events.timeout.last_read = t.tv_sec;
		peer->events.timeout.last_write = t.tv_sec;
		peer->events.timeout.in_timeout = io_thread->in_timeout_sec;
		peer->events.timeout.out_timeout = io_thread->out_timeout_sec;
		io_thread->peers.insert(peer);

		if (task.init_cb)
		{
			(*task.init_cb)(peer, &task);
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

#if WIN32
unsigned int peerIOThreadFunc(void *arg)
#else
void* peerIOThreadFunc(void *arg)
#endif
{
	// libevent use thread
#if WIN32
	if (evthread_use_windows_threads() != 0)
	{
		exit(EXIT_FAILURE);
	}
#else
	if (evthread_use_pthreads() != 0)
	{
		exit(EXIT_FAILURE);
	}
#endif

	struct PeerIOThreadArg *io_thread_arg = (struct PeerIOThreadArg*)arg;

	PeerIOThread *io_thread = new PeerIOThread;
	if (io_thread)
	{
		io_thread->thread_id = io_thread_arg->id;
		io_thread->base = event_base_new();
		if (io_thread->base)
		{
			if (bufferevent_pair_new(io_thread->base, BEV_OPT_CLOSE_ON_FREE, io_thread->tunnel) == 0)
			{
				bufferevent_setcb(io_thread->tunnel[TUNNEL_OUT], tunnelcb, NULL, NULL, (void*)io_thread);
				bufferevent_enable(io_thread->tunnel[TUNNEL_OUT], EV_READ);
				bufferevent_enable(io_thread->tunnel[TUNNEL_IN], EV_WRITE);
			}
		}
	}

	io_thread_arg->p_io_thread = io_thread;

#if WIN32
	EnterCriticalSection(&io_thread_arg->mtx);
	LeaveCriticalSection(&io_thread_arg->mtx);
	WakeConditionVariable(&io_thread_arg->cv);
#else
	pthread_mutex_lock(&io_thread_arg->mtx);
	pthread_mutex_unlock(&io_thread_arg->mtx);
	pthread_cond_signal(&io_thread_arg->cv);
#endif

	if (io_thread && io_thread->base)
	{
		event_base_loop(io_thread->base, EVLOOP_NO_EXIT_ON_EMPTY);
	}

#if WIN32
	return 0;
#else
	return NULL;
#endif
}

PeerIOThread* createPeerIOThread(int id)
{
	struct PeerIOThreadArg *arg = (struct PeerIOThreadArg*)malloc(sizeof(struct PeerIOThreadArg));
	arg->id = id;

#if WIN32
	InitializeCriticalSection(&arg->mtx);
	InitializeConditionVariable(&arg->cv);

	EnterCriticalSection(&arg->mtx);

	unsigned int system_thread_id;
	_beginthreadex(NULL, 0, &peerIOThreadFunc, (void*)arg, 0, &system_thread_id);

	SleepConditionVariableCS(&arg->cv, &arg->mtx, INFINITE);

	DeleteCriticalSection(&arg->mtx);
#else
	if (pthread_mutex_init(&arg->mtx, NULL) != 0)
	{
		free(arg);
		return NULL;
	}
	if (pthread_cond_init(&arg->cv, NULL) != 0)
	{
		free(arg);
		return NULL;
	}
	if (pthread_mutex_lock(&arg->mtx) != 0)
	{
		free(arg);
		return NULL;
	}

	pthread_t system_thread_id;
	pthread_create(&system_thread_id, NULL, peerIOThreadFunc, (void*)arg);
	pthread_detach(system_thread_id);

	pthread_cond_wait(&arg->cv, &arg->mtx);

	pthread_cond_destroy(&arg->cv);
	pthread_mutex_destroy(&arg->mtx);
#endif

	PeerIOThread *io_thread = (PeerIOThread*)arg->p_io_thread;
	if (io_thread != NULL)
	{
		io_thread->system_thread_id = system_thread_id;
	}

	free(arg);

	return io_thread;
}

int peerIOThreadSetTimeout(PeerIOThread *io_thread, long check_interval_sec,
	unsigned short in_timeout_sec, unsigned short out_timeout_sec)
{
	struct PeerIOThread_CheckTimeout task = { TUNNEL_ADD_TIME, { check_interval_sec, 0 } };
	io_thread->in_timeout_sec = in_timeout_sec;
	io_thread->out_timeout_sec = out_timeout_sec;
	int ret = bufferevent_write(io_thread->tunnel[TUNNEL_IN], (void*)&task, sizeof(struct PeerIOThread_CheckTimeout));

	return ret;
}
int peerIOThreadAddSocket(PeerIOThread *io_thread, struct PeerIOThread_AddSocket *task)
{
	task->type = TUNNEL_ADD_SOCKET;
	return bufferevent_write(io_thread->tunnel[TUNNEL_IN], (void*)task, sizeof(struct PeerIOThread_AddSocket));
}
int peerIOThreadGetPeerCnt(PeerIOThread *io_thread)
{
	return (int)io_thread->peers.size();
}
