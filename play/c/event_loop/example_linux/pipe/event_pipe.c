#include "event_pipe.h"

#define EVENT_PIPE_READ_END 0
#define EVENT_PIPE_WRITE_END 1

static int event_pipe_writen(int fd, void *ptr, size_t numbytes)
{
	size_t nleft = numbytes;
	const char *p = (const char*)ptr;
	ssize_t n = 0;
	do {
		n = write(fd, p, nleft);
		if (n < 0)
		{
			if (n < 0 && muggle_sys_lasterror() == EINTR)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}

		nleft -= n;
		p += n;
	} while (nleft > 0);

	return numbytes;
}

static int event_pipe_readall(int fd)
{
	uint64_t buf[128];
	int n = 0;
	int cnt = 0;
	do {
		n = read(fd, buf, sizeof(buf));
		if (n < 0)
		{
			int errnum = muggle_sys_lasterror();
			if (errnum == EWOULDBLOCK || errnum == EAGAIN)
			{
				break;
			}
			else if (errnum == EINTR)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		else if (n == 0)
		{
			// EOF
			return -1;
		}

		cnt += n / sizeof(uint64_t);
		if (n < sizeof(buf))
		{
			break;
		}
	} while (1);

	return cnt;
}

int event_pipe_init(event_pipe_t *ev_pipe)
{
	memset(ev_pipe, 0, sizeof(*ev_pipe));
	ev_pipe->fds[0] = -1;
	ev_pipe->fds[1] = -1;

	if (pipe(ev_pipe->fds) == -1)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed create pipe");
		goto event_pipe_init_except;
	}

	muggle_socket_set_nonblock(ev_pipe->fds[0], 1);
	muggle_socket_set_nonblock(ev_pipe->fds[1], 1);

	ev_pipe->mtx = (muggle_mutex_t*)malloc(sizeof(muggle_mutex_t));
	if (ev_pipe->mtx == NULL)
	{
		LOG_ERROR("failed allocate space for event pipe's mutex");
		goto event_pipe_init_except;
	}

	if (muggle_mutex_init(ev_pipe->mtx) != 0)
	{
		LOG_ERROR("failed init event pipe's mutex");
		free(ev_pipe->mtx);
		ev_pipe->mtx = NULL;

		goto event_pipe_init_except;
	}

	return 0;

event_pipe_init_except:
	event_pipe_destroy(ev_pipe);
	return -1;
}

void event_pipe_destroy(event_pipe_t *ev_pipe)
{
	if (ev_pipe->mtx)
	{
		muggle_mutex_destroy(ev_pipe->mtx);
		free(ev_pipe->mtx);
		ev_pipe->mtx = NULL;
	}

	if (ev_pipe->fds[0] != -1)
	{
		close(ev_pipe->fds[0]);
		ev_pipe->fds[0] = -1;
	}

	if (ev_pipe->fds[1] != -1)
	{
		close(ev_pipe->fds[1]);
		ev_pipe->fds[1] = -1;
	}
}

int event_pipe_wakeup(event_pipe_t *ev_pipe)
{
	uint64_t v = 0;
	muggle_mutex_lock(ev_pipe->mtx);
	int n = event_pipe_writen(ev_pipe->fds[EVENT_PIPE_WRITE_END], &v, sizeof(v));
	muggle_mutex_unlock(ev_pipe->mtx);

	return n == sizeof(v) ? 0 : -1;
}

int event_pipe_clearup(event_pipe_t *ev_pipe)
{
	muggle_mutex_lock(ev_pipe->mtx);
	int n = event_pipe_readall(ev_pipe->fds[EVENT_PIPE_READ_END]);
	muggle_mutex_unlock(ev_pipe->mtx);

	return n;
}
