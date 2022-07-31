#include "muggle/c/muggle_c.h"
#include <sys/eventfd.h>

#define PRODUCER_NUM 16

typedef struct message
{
	int thread_idx;  //!< thread index
	int msg_idx;     //!< message index
} message_t;

typedef struct thread_args
{
	int               thread_idx; //!< thread index
	int               evfd;      //!< event pipe
	muggle_atomic_int *completed; //!< task completed //!< task completed
	int               cnt;        //!< producer total send
} thread_args_t;

muggle_thread_ret_t producer_routine(void *arg)
{
	thread_args_t *p_arg = (thread_args_t*)arg;
	int evfd = p_arg->evfd;

	int msg_idx = 0;
	do {
		muggle_msleep(20);

		message_t *msg = (message_t*)malloc(sizeof(message_t));
		msg->thread_idx = p_arg->thread_idx;
		msg->msg_idx = ++msg_idx;

		LOG_INFO("producer write %d|%d", p_arg->thread_idx, msg_idx);

		uint64_t v = 1;
		if (write(evfd, &v, sizeof(v)) != sizeof(v))
		{
			MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed write into eventfd");
		}
	} while (muggle_atomic_load(p_arg->completed, muggle_memory_order_relaxed) == 0);

	LOG_INFO("producer exit, %d total write %d", p_arg->thread_idx, msg_idx);
	p_arg->cnt = msg_idx;

	return 0;
}

uint64_t clear_eventfd(int evfd)
{
	uint64_t v = 0;
	ssize_t n = read(evfd, &v, sizeof(v));
	if (n != sizeof(v))
	{
		if (n == -1)
		{
			int errnum = muggle_sys_lasterror();
			if (errnum == EWOULDBLOCK || errnum == EAGAIN)
			{
				LOG_INFO("eventfd read nothing");
			}
			else
			{
				MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed read eventfd");
			}
		}
		else if (n == 0)
		{
			LOG_ERROR("read EOF of eventfd");
		}

		v = 0;
	}

	return v;
}

uint64_t consumer_routine(int evfd)
{
	uint64_t total_recv = 0;
	while (1)
	{
		muggle_msleep(500);

		uint64_t v = clear_eventfd(evfd);
		if (v != 0)
		{
			total_recv += v;
			LOG_INFO("read %llu, total %llu",
				(unsigned long long)v, (unsigned long long)total_recv);
		}

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

	// init eventfd
	int evfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
	if (evfd == -1)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed create eventfd");
		exit(EXIT_FAILURE);
	}
	
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
		producer_infos[i].evfd = evfd;
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
	uint64_t total_recv = consumer_routine(evfd);

	// notify producer exit
	muggle_atomic_store(&completed, 1, muggle_memory_order_relaxed);

	// join and exit
	int producer_total_send = 0;
	for (int i = 0; i < sizeof(th_producers)/sizeof(th_producers[0]); i++)
	{
		ret = muggle_thread_join(&th_producers[i]);
		if (ret != 0)
		{
			MUGGLE_LOG_ERROR("failed thread join");
			exit(EXIT_FAILURE);
		}
		producer_total_send += producer_infos[i].cnt;
	}

	// exhaust eventfd
	total_recv += clear_eventfd(evfd);

	// close eventfd
	close(evfd);

	LOG_INFO("producer total send: %d, consumer total recv: %d",
		producer_total_send, total_recv);

	return 0;
}
