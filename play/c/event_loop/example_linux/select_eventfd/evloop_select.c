#include "evloop_select.h"
#include <sys/eventfd.h>

static void evloop_handle_timer(ev_loop_t *evloop)
{
	if (evloop->timer_cb == NULL)
	{
		return;
	}

	struct timespec curr_ts;
	timespec_get(&curr_ts, TIME_UTC);
	int interval_ms = 
		(int)((curr_ts.tv_sec - evloop->last_ts.tv_sec) * 1000 +
		(curr_ts.tv_nsec - evloop->last_ts.tv_nsec) / 1000000);
	if (interval_ms >= evloop->timeout)
	{
		evloop->timer_cb(evloop, NULL);
		evloop->last_ts.tv_sec = curr_ts.tv_sec;
		evloop->last_ts.tv_nsec = curr_ts.tv_nsec;
	}
}

int evloop_init(ev_loop_t *evloop, int hints_max_fd, int use_mem_pool)
{
	memset(evloop, 0, sizeof(*evloop));

	FD_ZERO(&evloop->allset);
	evloop->evfd_ctx.fd = -1;

	// set hints max fd
	if (hints_max_fd < 1)
	{
		hints_max_fd = 8;
	}

	// linked list
	evloop->linked_list = (muggle_linked_list_t*)malloc(sizeof(muggle_linked_list_t));
	if (evloop->linked_list == NULL)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed malloc linked list");
		goto init_evloop_except;
	}

	// initialize linked list
	size_t capacity = 0;
	if (use_mem_pool)
	{
		capacity = hints_max_fd;
	}
	else
	{
		capacity = 0;
	}

	if (!muggle_linked_list_init(evloop->linked_list, capacity))
	{
		LOG_ERROR("failed init linked list");
		free(evloop->linked_list);
		evloop->linked_list = NULL;
		goto init_evloop_except;
	}

	// create eventfd
	evloop->evfd_ctx.fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
	if (evloop->evfd_ctx.fd == -1)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed eventfd");
		goto init_evloop_except;
	}

	// set eventfd into allset
	FD_SET(evloop->evfd_ctx.fd, &evloop->allset);
	evloop->nfds = evloop->evfd_ctx.fd;

	// without timeout by default
	evloop->timeout = -1;

	// before run, tmp set thread
	evloop->loop_thread = muggle_thread_current_id();

	return 0;

init_evloop_except:
	evloop_destroy(evloop);

	return -1;
}

void evloop_destroy(ev_loop_t *evloop)
{
	if (evloop->evfd_ctx.fd != -1)
	{
		close(evloop->evfd_ctx.fd);
		evloop->evfd_ctx.fd = -1;
	}

	if (evloop->linked_list)
	{
		muggle_linked_list_destroy(evloop->linked_list, NULL, NULL);
		evloop->linked_list = NULL;
	}

	FD_ZERO(&evloop->allset);
}

void evloop_set_timer(ev_loop_t *evloop, int timeout)
{
	evloop->timeout = timeout;
}

void evloop_set_cb_read(ev_loop_t *evloop, fn_evloop_callback cb)
{
	evloop->read_cb = cb;
}

void evloop_set_cb_error(ev_loop_t *evloop, fn_evloop_callback cb)
{
	evloop->err_cb = cb;
}

void evloop_set_cb_wakeup(ev_loop_t *evloop, fn_evloop_callback cb, void *data)
{
	evloop->wake_cb = cb;
	evloop->evfd_ctx.data = data;
}

void evloop_set_cb_timer(ev_loop_t *evloop, fn_evloop_callback cb)
{
	evloop->timer_cb = cb;
}

void evloop_wakeup(ev_loop_t *evloop)
{
	uint64_t v = 1;

	// don't need use lock here, write eventfd is thread safe
	if (write(evloop->evfd_ctx.fd, &v, sizeof(uint64_t)) != sizeof(uint64_t))
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "write eventfd");
	}
}

int evloop_add_ctx(ev_loop_t *evloop, ev_context_t *ctx)
{
	// only support add context in the same thread of event loop run
	if (!muggle_thread_equal(evloop->loop_thread, muggle_thread_current_id()))
	{
		return -1;
	}

	muggle_linked_list_node_t *node = muggle_linked_list_append(evloop->linked_list, NULL, ctx);
	if (node == NULL)
	{
		return -1;
	}

	FD_SET(ctx->fd, &evloop->allset);
	if (ctx->fd > evloop->nfds)
	{
		evloop->nfds = ctx->fd;
	}

	return 0;
}

void evloop_run(ev_loop_t *evloop)
{
	// prepare timer
	struct timeval timeout, save_timeout;
	struct timeval *p_timeout = &timeout;
	memset(&timeout, 0, sizeof(timeout));
	if (evloop->timeout < 0)
	{
		p_timeout = NULL;
	}
	else
	{
		timeout.tv_sec = evloop->timeout / 1000;
		timeout.tv_usec = (evloop->timeout % 1000) * 1000;
		memcpy(&save_timeout, &timeout, sizeof(struct timeval));
	}

	struct timespec t1, t2;
	if (evloop->timeout > 0)
	{
		timespec_get(&t1, TIME_UTC);
	}

	// set fds
	fd_set rset;
	fd_set eset;
	muggle_linked_list_t *linked_list = evloop->linked_list;
	muggle_linked_list_node_t *node = NULL;

	// run loop
	while (1)
	{
		// reset timeout
		if (p_timeout)
		{
			memcpy(&timeout, &save_timeout, sizeof(struct timeval));
		}

		// select loop
		rset = evloop->allset;
		eset = evloop->allset;
		int n = select(evloop->nfds + 1, &rset, NULL, &eset, p_timeout);
		if (n > 0)
		{
			FD_ZERO(&evloop->allset);

			// handle wakeup
			if (FD_ISSET(evloop->evfd_ctx.fd, &rset))
			{
				uint64_t v = 0;
				ssize_t n = read(evloop->evfd_ctx.fd, &v, sizeof(v));
				if (n != sizeof(v))
				{
					MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed read eventfd");
				}

				if (evloop->wake_cb)
				{
					evloop->wake_cb(evloop, &evloop->evfd_ctx);
				}
			}
			FD_SET(evloop->evfd_ctx.fd, &evloop->allset);
			evloop->nfds = evloop->evfd_ctx.fd;

			// handle read and except
			node = muggle_linked_list_first(linked_list);
			while (node)
			{
				ev_context_t *ctx = (ev_context_t*)node->data;
				if (FD_ISSET(ctx->fd, &rset))
				{
					if (evloop->read_cb)
					{
						evloop->read_cb(evloop, ctx);
					}
				}

				if (FD_ISSET(ctx->fd, &eset))
				{
					node = muggle_linked_list_remove(linked_list, node, NULL, NULL);
					if (evloop->err_cb)
					{
						evloop->err_cb(evloop, ctx);
					}
				}
				else
				{
					FD_SET(ctx->fd, &evloop->allset);
					if (ctx->fd > evloop->nfds)
					{
						evloop->nfds = ctx->fd;
					}
					node = muggle_linked_list_next(linked_list, node);
				}
			}

			// when loop is busy, timeout will not trigger, use
			// customize timer handle avoid that
			if (evloop->timeout > 0)
			{
				evloop_handle_timer(evloop);
			}
		}
		else if (n == 0)
		{
			evloop_handle_timer(evloop);
		}
		else
		{
			MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed select");
			evloop->exit = 1;
		}

		if (evloop->exit)
		{
			break;
		}
	}
}
