#include "peer_io_thread.h"
#include <event2/thread.h>

#if WIN32
#define ThreadLocal __declspec(thread) 
#else
#define ThreadLocal __thread
#endif

enum eTunnelType
{
	TUNNEL_IN = 0,
	TUNNEL_OUT = 1
};

enum eTunnelInputType
{
	TUNNEL_ADD_TIME = 0,
	TUNNEL_ADD_STR = 1,
	TUNNEL_ADD_SOCKET = 2,
};

struct PeerIOThreadArg
{
	volatile struct PeerIOThread *p_io_thread;
	int id;
	int max_peer_num;
#if WIN32
	CRITICAL_SECTION mtx;
	CONDITION_VARIABLE cv;
#else
	pthread_mutex_t mtx;
	pthread_cond_t cond_var;
#endif
};

struct PeerIOThread_TimeTask
{
	PeerIOThreadTaskType type;
	struct timeval time_interval;
};
struct PeerIOThread_StringTask
{
	PeerIOThreadTaskType type;
	int len;
	char buf[128];
};

// internal functions
void check_timeout(evutil_socket_t fd, short events, void *arg);
void tunnelcb(struct bufferevent *bev, void *ctx);
int tunnelcb_timetask(struct evbuffer *input, struct PeerIOThread *io_thread);
int tunnelcb_stringtask(struct evbuffer *input, struct PeerIOThread *io_thread);
int tunnelcb_addsocktask(struct evbuffer *input, struct PeerIOThread *io_thread);

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

	struct PeerIOThread *io_thread = malloc(sizeof(struct PeerIOThread));
	if (io_thread)
	{
		memset(io_thread, 0, sizeof(struct PeerIOThread));
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

struct PeerIOThread* createPeerIOThread(int id, int max_peer_num)
{
	struct PeerIOThreadArg *arg = (struct PeerIOThreadArg*)malloc(sizeof(struct PeerIOThreadArg));
	arg->id = id;
	arg->max_peer_num = max_peer_num;

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
		free(peer_io_th);
		return NULL;
	}
	if (pthread_cond_init(&arg->cv, NULL) != 0)
	{
		free(peer_io_th);
		return NULL;
	}
	if (pthread_mutex_lock(&arg->mtx) != 0)
	{
		free(peer_io_th);
		return NULL;
	}

	pthread_t system_thread_id;
	pthread_create(&system_thread_id, NULL, threadFunc, (void*)arg);
	pthread_detach(system_thread_id);

	pthread_cond_wait(&arg->cv, &arg->mtx);

	pthread_cond_destroy(&arg->cv);
	pthread_mutex_destroy(&arg->mtx);
#endif

	struct PeerIOThread *io_thread = arg->p_io_thread;
	if (io_thread != NULL)
	{
		io_thread->system_thread_id = system_thread_id;
	}

	free(arg);

	return io_thread;
}

int peerIOThreadSetTimeout(struct PeerIOThread* io_thread, long timeout_sec, long check_interval_sec)
{
	struct PeerIOThread_TimeTask task = { TUNNEL_ADD_TIME, { check_interval_sec, 0 } };
	int ret = bufferevent_write(io_thread->tunnel[TUNNEL_IN], (void*)&task, sizeof(struct PeerIOThread_TimeTask));
	io_thread->timeout_sec = timeout_sec;

	return ret;
}
int peerIOThreadAddSocket(struct PeerIOThread *io_thread, struct PeerIOThread_AddSocketTask *task)
{
	task->type = TUNNEL_ADD_SOCKET;
	return bufferevent_write(io_thread->tunnel[TUNNEL_IN], (void*)task, sizeof(struct PeerIOThread_AddSocketTask));
}
int peerIOThreadGetPeerCnt(struct PeerIOThread *io_thread)
{
	return 0;
}

void check_timeout(evutil_socket_t fd, short events, void *arg)
{
	struct PeerIOThread *io_thread = (struct PeerIOThread*)arg;
	// fprintf(stdout, "io thread[%d]: check timeout\n", io_thread->thread_id);
	// TODO: 
}
void tunnelcb(struct bufferevent *bev, void *ctx)
{
	struct PeerIOThread *io_thread = (struct PeerIOThread*)ctx;
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
		case TUNNEL_ADD_STR:
		{
			ret = tunnelcb_stringtask(input, io_thread);
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
int tunnelcb_timetask(struct evbuffer *input, struct PeerIOThread *io_thread)
{
	struct PeerIOThread_TimeTask task;
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

	if (task.time_interval.tv_sec > 0 || task.time_interval.tv_usec > 0)
	{
		struct event *ev = event_new(io_thread->base, -1, EV_PERSIST, check_timeout, (void*)io_thread);
		if (!ev)
		{
			return -2;
		}

		if (event_add(ev, &task.time_interval) < 0)
		{
			event_free(ev);
			return -3;
		}

		io_thread->timeout_ev = ev;
	}

	return 0;
}
int tunnelcb_stringtask(struct evbuffer *input, struct PeerIOThread *io_thread)
{
	struct PeerIOThread_StringTask task;
	if (evbuffer_get_length(input) >= sizeof(task))
	{
		evbuffer_remove(input, &task, sizeof(task));
	}
	else
	{
		return -1;
	}

	fprintf(stdout, "test tunnel string: %s\n", task.buf);

	return 0;
}
int tunnelcb_addsocktask(struct evbuffer *input, struct PeerIOThread *io_thread)
{
	struct PeerIOThread_AddSocketTask task;
	if (evbuffer_get_length(input) >= sizeof(task))
	{
		evbuffer_remove(input, &task, sizeof(task));
		struct Peer *peer = peerConnectIn(io_thread->base, task.fd, &task.ss, task.slen, task.peer_size);
		if (!peer)
		{
			return -1;
		}

		if (task.init_cb)
		{
			(*task.init_cb)(peer, &task);
		}

		peerSetCb(peer, task.read_cb, task.write_cb, task.event_cb);
	}
	else
	{
		return -1;
	}

	return 0;
}