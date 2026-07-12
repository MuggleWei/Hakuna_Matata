#include "serv.h"
#include "data.h"

static muggle_thread_ret_t tcp_listen_routine(void *p_args)
{
	muggle_event_loop_t *evloop = (muggle_event_loop_t *)p_args;
	evloop_data_t *evloop_data =
		(evloop_data_t *)muggle_evloop_get_data(evloop);
	cfg_t *cfg = evloop_data->cfg;

	muggle_socket_t fd = MUGGLE_INVALID_SOCKET;
	do {
		LOG_INFO("tcp listen: %s %s", cfg->host, cfg->port);
		fd = muggle_tcp_listen_with_cb(cfg->host, cfg->port, 512,
									   set_socket_opt_bufsize, cfg);
		if (fd == MUGGLE_INVALID_SOCKET) {
			LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed listen");
			muggle_msleep(3000);
			continue;
		}
		break;
	} while (1);

	LOG_INFO("success tcp listen");

	muggle_socket_context_t *ctx =
		(muggle_socket_context_t *)malloc(sizeof(muggle_socket_context_t));
	muggle_socket_ctx_init(ctx, fd, NULL, MUGGLE_SOCKET_CTX_TYPE_TCP_LISTEN);

	muggle_socket_evloop_add_ctx(evloop, ctx);

	return 0;
}

void run_server(muggle_event_loop_t *evloop,
				muggle_socket_evloop_handle_t *handle)
{
	muggle_socket_evloop_handle_set_cb_add_ctx(handle, serv_on_add_ctx);
	muggle_socket_evloop_handle_set_cb_conn(handle, serv_on_connect);
	muggle_socket_evloop_handle_set_cb_msg(handle, serv_on_message);
	muggle_socket_evloop_handle_set_cb_close(handle, serv_on_close);
	muggle_socket_evloop_handle_set_cb_release(handle, serv_on_release);
	muggle_socket_evloop_handle_attach(handle, evloop);
	LOG_INFO("socket handle attached to event loop");

	// run listen
	muggle_thread_t th;
	muggle_thread_create(&th, tcp_listen_routine, evloop);
	muggle_thread_detach(&th);

	// run
	muggle_evloop_run(evloop);
}

void serv_on_add_ctx(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	gen_session_info(ctx);
	session_data_t *session_data = get_session_info(ctx);

	LOG_INFO("listen session add into evloop, "
			 "local_ip=%s, local_port=%d, rcvbuf=%u, sndbuf=%u",
			 session_data->local_ip, session_data->local_port,
			 session_data->rcv_bufsize, session_data->snd_bufsize);
}
void serv_on_connect(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	gen_session_info(ctx);
	session_data_t *session_data = get_session_info(ctx);

	LOG_INFO("client session connection, "
			 "remote_ip=%s, remote_port=%d, rcvbuf=%u, sndbuf=%u",
			 session_data->remote_ip, session_data->remote_port,
			 session_data->rcv_bufsize, session_data->snd_bufsize);
}
void serv_on_message(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	int n = 0;
	char buf[65536];
	while ((n = muggle_socket_ctx_read(ctx, buf, sizeof(buf))) > 0) {
		muggle_socket_block_write(ctx->base.fd, buf, n, 40);
	}
}
void serv_on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	session_data_t *session_data = get_session_info(ctx);
	LOG_INFO("client session close, "
			 "remote_ip=%s, remote_port=%d",
			 session_data->remote_ip, session_data->remote_port);
}
void serv_on_release(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	session_data_t *session_data = get_session_info(ctx);
	LOG_INFO("client session release, "
			 "remote_ip=%s, remote_port=%d",
			 session_data->remote_ip, session_data->remote_port);

	release_session_info(ctx);
}
