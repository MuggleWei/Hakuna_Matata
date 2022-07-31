#include "event_pipe.h"

#define PRODUCER_NUM 16

typedef struct message
{
	int thread_idx;  //!< thread index
	int msg_idx;     //!< message index
} message_t;

typedef struct thread_args
{
	int               thread_idx; //!< thread index
	event_pipe_t      *ev_pipe;   //!< event pipe
	muggle_atomic_int *completed; //!< task completed //!< task completed
	int               cnt;        //!< producer total send
} thread_args_t;

muggle_thread_ret_t producer_routine(void *arg)
{
	thread_args_t *p_arg = (thread_args_t*)arg;
	event_pipe_t *ev_pipe = p_arg->ev_pipe;

	int msg_idx = 0;
	do {
		muggle_msleep(20);

		message_t *msg = (message_t*)malloc(sizeof(message_t));
		msg->thread_idx = p_arg->thread_idx;
		msg->msg_idx = ++msg_idx;

		LOG_INFO("producer write %d|%d", p_arg->thread_idx, msg_idx);

		event_pipe_wakeup(ev_pipe);
	} while (muggle_atomic_load(p_arg->completed, muggle_memory_order_relaxed) == 0);

	LOG_INFO("producer exit, %d total write %d", p_arg->thread_idx, msg_idx);
	p_arg->cnt = msg_idx;

	return 0;
}

uint64_t consumer_routine(event_pipe_t *ev_pipe)
{
	LOG_INFO("start consumer");

	uint64_t total_recv = 0;
	uint64_t buf[128];
	while (1)
	{
		muggle_msleep(500);

		LOG_INFO("start clearup");

		int n = event_pipe_clearup(ev_pipe);
		if (n == -1)
		{
			LOG_ERROR("failed clearup event pipe");
			break;
		}

		total_recv += n;
		LOG_INFO("read %llu, total %llu",
			(unsigned long long)n, (unsigned long long)total_recv);

		if (total_recv >= 512)
		{
			break;
		}
	}

	return total_recv;
}

int main(int argc, char *argv[])
{
	// init log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// init event pipe
	event_pipe_t ev_pipe;
	memset(&ev_pipe, 0, sizeof(ev_pipe));
	event_pipe_init(&ev_pipe);

	// complete flags
	muggle_atomic_int completed = 0;

	// producers
	int ret = 0;
	muggle_thread_t th_producers[PRODUCER_NUM];
	thread_args_t producer_infos[PRODUCER_NUM];
	for (int i = 0; i < sizeof(th_producers)/sizeof(th_producers[0]); i++)
	{
		memset(&producer_infos[i], 0, sizeof(producer_infos[i]));
		producer_infos[i].thread_idx = i;
		producer_infos[i].ev_pipe = &ev_pipe;
		producer_infos[i].completed = &completed;
		producer_infos[i].cnt = 0;

		ret = muggle_thread_create(&th_producers[i], producer_routine, &producer_infos[i]);
		if (ret != 0)
		{
			MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed create thread");
			exit(EXIT_FAILURE);
		}
	}

	// consumer
	int total_recv = consumer_routine(&ev_pipe);

	// notify producer exit
	muggle_atomic_store(&completed, 1, muggle_memory_order_relaxed);

	// join and exit
	int producer_total_send = 0;
	for (int i = 0; i < sizeof(th_producers)/sizeof(th_producers[0]); i++)
	{
		ret = muggle_thread_join(&th_producers[i]);
		if (ret != 0)
		{
			char buf[512];
			muggle_sys_strerror(muggle_sys_lasterror(), buf, sizeof(buf));
			LOG_ERROR("failed join thread: %s", buf);
			exit(EXIT_FAILURE);
		}
		producer_total_send += producer_infos[i].cnt;
	}

	// exhaust pipe
	int n = event_pipe_clearup(&ev_pipe);
	if (n == -1)
	{
		LOG_ERROR("failed event pipe clearup");
	}
	else
	{
		total_recv += n;
	}

	// destroy event_pipe
	event_pipe_destroy(&ev_pipe);

	LOG_INFO("producer total send: %d, consumer total recv: %d",
		producer_total_send, total_recv);

	return 0;
}
