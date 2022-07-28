#include "evloop.h"

#define PRODUCER_NUM 16

typedef struct message
{
	int thread_idx;  //!< thread index
	int msg_idx;     //!< message index
} message_t;

typedef struct thread_args
{
	int               thread_idx; //!< thread index
	ev_loop_t         *evloop;    //!< event loop
	muggle_queue_t    *queue;     //!< message queue
	muggle_mutex_t    *mtx;       //!< mutex for message queue
	muggle_atomic_int *completed; //!< task completed
	int                cnt;       //!< producer total send
} thread_args_t;

typedef struct consumer_info
{
	int            cnt;    //!< recv message count
	muggle_queue_t *queue; //!< message queue
	muggle_mutex_t *mtx;   //!< mutex for message queue
} consumer_info_t;

muggle_thread_ret_t producer_routine(void *arg)
{
	thread_args_t *p_arg = (thread_args_t*)arg;
	ev_loop_t *evloop = p_arg->evloop;
	muggle_queue_t *queue = p_arg->queue;
	muggle_mutex_t *mtx = p_arg->mtx;

	int msg_idx = 0;
	do {
		muggle_msleep(20);

		message_t *msg = (message_t*)malloc(sizeof(message_t));
		msg->thread_idx = p_arg->thread_idx;
		msg->msg_idx = ++msg_idx;

		LOG_INFO("producer write %d|%d", p_arg->thread_idx, msg_idx);

		muggle_mutex_lock(mtx);
		if (muggle_queue_enqueue(queue, (void*)msg) == NULL)
		{
			LOG_ERROR("producer failed push %d|%d");
			free(msg);
			continue;
		}
		muggle_mutex_unlock(mtx);

		evloop_wakeup(evloop);
	} while (muggle_atomic_load(p_arg->completed, muggle_memory_order_relaxed) == 0);

	LOG_INFO("producer exit, %d total write %d", p_arg->thread_idx, msg_idx);
	p_arg->cnt = msg_idx;

	return 0;
}

void on_error(struct ev_loop *evloop, struct ev_context *ctx)
{
	LOG_ERROR("on_error");
}

void on_wakeup(struct ev_loop *evloop, struct ev_context *ctx)
{
	consumer_info_t *info = ctx->data;

	muggle_mutex_lock(info->mtx);
	while (!muggle_queue_is_empty(info->queue))
	{
		++info->cnt;

		muggle_queue_node_t *queue_node = muggle_queue_front(info->queue);
		message_t *msg = (message_t*)queue_node->data;
		muggle_queue_dequeue(info->queue, NULL, NULL);
		LOG_INFO("recv %d|%d, total %d", msg->thread_idx, msg->msg_idx, info->cnt);
		free(msg);

		if (info->cnt >= 512)
		{
			LOG_INFO("consumer exit");
			evloop->exit = 1;
			break;
		}
	}
	muggle_mutex_unlock(info->mtx);
}

int main(int argc, char *argv[])
{
	// init log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	int ret = 0;

	// complete flags
	muggle_atomic_int completed = 0;

	// create queue and mutex
	muggle_queue_t queue;
	if (!muggle_queue_init(&queue, 0))
	{
		LOG_ERROR("failed init queue");
		exit(EXIT_FAILURE);
	}

	muggle_mutex_t mtx;
	if (muggle_mutex_init(&mtx) != 0)
	{
		LOG_ERROR("failed init mutex");
		exit(EXIT_FAILURE);
	}

	consumer_info_t info;
	memset(&info, 0, sizeof(info));
	info.cnt = 0;
	info.queue = &queue;
	info.mtx = &mtx;

	// create event loop
	ev_loop_t evloop;
	if (evloop_init(&evloop) != 0)
	{
		LOG_ERROR("failed init event loop");
		exit(EXIT_FAILURE);
	}
	evloop_set_cb_error(&evloop, on_error);
	evloop_set_cb_wakeup(&evloop, on_wakeup, &info);

	// producers
	muggle_thread_t th_producers[PRODUCER_NUM];
	thread_args_t producer_infos[PRODUCER_NUM];
	for (int i = 0; i < sizeof(th_producers)/sizeof(th_producers[0]); i++)
	{
		memset(&producer_infos[i], 0, sizeof(producer_infos[i]));
		producer_infos[i].thread_idx = i;
		producer_infos[i].evloop = &evloop;
		producer_infos[i].queue = &queue;
		producer_infos[i].mtx = &mtx;
		producer_infos[i].completed = &completed;

		ret = muggle_thread_create(&th_producers[i], producer_routine, &producer_infos[i]);
		if (ret != 0)
		{
			char buf[512];
			muggle_sys_strerror(muggle_sys_lasterror(), buf, sizeof(buf));
			LOG_ERROR("failed create thread: %s", buf);
			exit(EXIT_FAILURE);
		}
	}

	// run event loop
	evloop_run(&evloop);

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

	// exhaust queue
	while (!muggle_queue_is_empty(&queue))
	{
		++info.cnt;
		muggle_queue_node_t *queue_node = muggle_queue_front(&queue);
		message_t *msg = (message_t*)queue_node->data;
		muggle_queue_dequeue(&queue, NULL, NULL);
		LOG_INFO("exhaust queue, recv %d|%d", msg->thread_idx, msg->msg_idx);
		free(msg);
	}

	// destroy queue and mutex
	muggle_queue_dequeue(&queue, NULL, NULL);
	muggle_mutex_destroy(&mtx);

	LOG_INFO("producer total send: %d, consumer total recv: %d",
		producer_total_send, info.cnt);

	return 0;
}
