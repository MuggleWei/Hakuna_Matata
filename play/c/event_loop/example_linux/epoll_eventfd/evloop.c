#include "evloop.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>

static void sys_err(const char *msg)
{
	char buf[512];
	muggle_sys_strerror(muggle_sys_lasterror(), buf, sizeof(buf));
	LOG_ERROR("%s: %s", msg, buf);
}

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

int evloop_init(ev_loop_t *evloop)
{
	memset(evloop, 0, sizeof(*evloop));
	evloop->epfd = -1;
	evloop->evfd_ctx.fd = -1;

	// create epoll fd
	evloop->epfd = epoll_create1(EPOLL_CLOEXEC);
	if (evloop->epfd == -1)
	{
		sys_err("failed epoll_create1");
		goto init_evloop_except;
	}

	// create eventfd
	evloop->evfd_ctx.fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
	if (evloop->evfd_ctx.fd == -1)
	{
		sys_err("failed eventfd");
		goto init_evloop_except;
	}

	struct epoll_event evfd_event;
	memset(&evfd_event, 0, sizeof(evfd_event));
	evfd_event.events = EPOLLIN | EPOLLET;
	evfd_event.data.ptr = &evloop->evfd_ctx;
	if (epoll_ctl(evloop->epfd, EPOLL_CTL_ADD, evloop->evfd_ctx.fd, &evfd_event) != 0)
	{
		sys_err("failed epoll_ctl add eventfd");
		goto init_evloop_except;
	}

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
	if (evloop->epfd != -1)
	{
		close(evloop->epfd);
		evloop->epfd = -1;
	}

	if (evloop->evfd_ctx.fd != -1)
	{
		close(evloop->evfd_ctx.fd);
		evloop->evfd_ctx.fd = -1;
	}
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
		sys_err("write eventfd");
	}
}

int evloop_add_ctx(ev_loop_t *evloop, ev_context_t *ctx)
{
	// only support add context in the same thread of event loop run
#if MUGGLE_PLATFORM_WINDOWS
	if (evloop->loop_thread != muggle_thread_current_id())
	{
		return -1;
	}
#else
	if (!pthread_equal(evloop->loop_thread, muggle_thread_current_id()))
	{
		return -1;
	}
#endif

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.data.ptr = ctx;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(evloop->epfd, EPOLL_CTL_ADD, ctx->fd, &event) != 0)
	{
		sys_err("failed epoll_ctl add");
		return -1;
	}

	return 0;
}

void evloop_run(ev_loop_t *evloop)
{
	// reset run thread
	evloop->loop_thread = muggle_thread_current_id();

	struct epoll_event events[64];
	while (1)
	{
		int nfds = epoll_wait(evloop->epfd, events, sizeof(events)/sizeof(events[0]), -1);
		if (nfds > 0)
		{
			for (int i = 0; i < nfds; i++)
			{
				ev_context_t *ctx = (ev_context_t*)events[i].data.ptr;
				if (events[i].events & EPOLLIN)
				{
					if (ctx == &evloop->evfd_ctx)
					{
						uint64_t v = 0;
						ssize_t n = read(ctx->fd, &v, sizeof(v));
						if (n != sizeof(v))
						{
							sys_err("failed read eventfd");
						}

						if (evloop->wake_cb)
						{
							evloop->wake_cb(evloop, ctx);
						}
					}
					else
					{
						if (evloop->read_cb)
						{
							evloop->read_cb(evloop, ctx);
						}
					}
				}
				else if (events[i].events & (EPOLLERR | EPOLLHUP))
				{
					epoll_ctl(evloop->epfd, EPOLL_CTL_DEL, ctx->fd, &events[i]);
					if (evloop->err_cb)
					{
						evloop->err_cb(evloop, ctx);
					}
				}
			}

			// when loop is busy, timeout will not trigger, use
			// customize timer handle avoid that
			if (evloop->timeout > 0)
			{
				evloop_handle_timer(evloop);
			}
		}
		else if (nfds == 0)
		{
			evloop_handle_timer(evloop);
		}
		else
		{
			sys_err("failed epoll_wait");
			evloop->exit = 1;
		}

		if (evloop->exit)
		{
			break;
		}
	}
}
