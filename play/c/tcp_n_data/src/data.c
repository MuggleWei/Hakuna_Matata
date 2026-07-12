#include "data.h"

void gen_session_info(muggle_socket_context_t *ctx)
{
	session_data_t *session_data =
		(session_data_t *)malloc(sizeof(session_data_t));
	memset(session_data, 0, sizeof(*session_data));

	muggle_event_fd fd = muggle_socket_ctx_get_fd(ctx);

	muggle_socket_local_ip_port(fd, session_data->local_ip,
								sizeof(session_data->local_ip),
								&session_data->local_port);
	muggle_socket_remote_ip_port(fd, session_data->remote_ip,
								 sizeof(session_data->remote_ip),
								 &session_data->remote_port);

	muggle_socklen_t len_rcv_bufsize = sizeof(session_data->rcv_bufsize);
	muggle_getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &session_data->rcv_bufsize,
					  &len_rcv_bufsize);

	muggle_socklen_t len_snd_bufsize = sizeof(session_data->snd_bufsize);
	muggle_getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &session_data->snd_bufsize,
					  &len_snd_bufsize);

	muggle_socket_ctx_set_data(ctx, session_data);

	int enable = 1;
	if (muggle_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable,
						  sizeof(enable)) != 0) {
		LOG_ERROR("failed set tcp nodelay");
		muggle_socket_ctx_shutdown(ctx);
		return;
	}
}

session_data_t *get_session_info(muggle_socket_context_t *ctx)
{
	return (session_data_t *)muggle_socket_ctx_get_data(ctx);
}

void release_session_info(muggle_socket_context_t *ctx)
{
	session_data_t *session_data =
		(session_data_t *)muggle_socket_ctx_get_data(ctx);
	if (session_data) {
		if (session_data->datas) {
			free(session_data->datas);
			session_data->datas = NULL;
		}

		free(session_data);
		muggle_socket_ctx_set_data(ctx, NULL);
	}
}

void set_socket_opt_bufsize(muggle_socket_t fd, void *p)
{
	cfg_t *cfg = (cfg_t *)p;
	int rcv_bufsize = cfg->rcvbuf;
	int snd_bufsize = cfg->sndbuf;

	if (rcv_bufsize > 0) {
		int default_rcv_bufsize = 0;
		muggle_socklen_t len_rcv_bufsize = sizeof(default_rcv_bufsize);
		muggle_getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &default_rcv_bufsize,
						  &len_rcv_bufsize);
		if (default_rcv_bufsize < rcv_bufsize) {
			if (muggle_setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcv_bufsize,
								  sizeof(rcv_bufsize)) != 0) {
				LOG_ERROR("failed set socket rcv buffer size");
			}
		}
	}

	if (snd_bufsize > 0) {
		int default_snd_bufsize = 0;
		muggle_socklen_t len_snd_bufsize = sizeof(default_snd_bufsize);
		muggle_getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &default_snd_bufsize,
						  &len_snd_bufsize);
		if (default_snd_bufsize < snd_bufsize) {
			if (muggle_setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &snd_bufsize,
								  sizeof(snd_bufsize)) != 0) {
				LOG_ERROR("failed set socket snd buffer size");
			}
		}
	}
}
