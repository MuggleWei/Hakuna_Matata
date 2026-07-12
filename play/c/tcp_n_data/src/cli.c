#include "cli.h"
#include "data.h"
#include "snd_method.h"

muggle_thread_ret_t conn_thread_routine(void *p_args)
{
	muggle_event_loop_t *evloop = (muggle_event_loop_t *)p_args;
	evloop_data_t *evloop_data =
		(evloop_data_t *)muggle_evloop_get_data(evloop);
	cfg_t *cfg = evloop_data->cfg;

	while (1) {
		// create socket fd
		muggle_socket_t fd = MUGGLE_INVALID_SOCKET;
		fd = muggle_tcp_connect_with_cb(cfg->host, cfg->port, 3,
										set_socket_opt_bufsize, cfg);

		if (fd == MUGGLE_INVALID_SOCKET) {
			LOG_ERROR("failed create socket: %s %s", cfg->host, cfg->port);
			muggle_msleep(3000);
			continue;
		}

		// create socket context
		muggle_socket_context_t *ctx =
			(muggle_socket_context_t *)malloc(sizeof(muggle_socket_context_t));
		muggle_socket_ctx_init(ctx, fd, NULL,
							   MUGGLE_SOCKET_CTX_TYPE_TCP_CLIENT);

		// add context to event loop
		muggle_socket_evloop_add_ctx(evloop, ctx);

		// exit connect thread
		break;
	}

	return 0;
}

void run_client(muggle_event_loop_t *evloop,
				muggle_socket_evloop_handle_t *handle)
{
	muggle_socket_evloop_handle_set_cb_add_ctx(handle, client_on_add_ctx);
	muggle_socket_evloop_handle_set_cb_close(handle, client_on_close);
	muggle_socket_evloop_handle_set_cb_msg(handle, client_on_message);
	muggle_socket_evloop_handle_set_cb_release(handle, client_on_release);
	muggle_socket_evloop_handle_attach(handle, evloop);
	LOG_INFO("socket handle attached to event loop");

	// connect
	muggle_thread_t th;
	muggle_thread_create(&th, conn_thread_routine, evloop);
	muggle_thread_detach(&th);

	// run
	muggle_evloop_run(evloop);
}

void client_on_add_ctx(muggle_event_loop_t *evloop,
					   muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	gen_session_info(ctx);
	session_data_t *session_data = get_session_info(ctx);

	evloop_data_t *evloop_data =
		(evloop_data_t *)muggle_evloop_get_data(evloop);
	cfg_t *cfg = evloop_data->cfg;
	session_data->cfg = cfg;
	session_data->total_data_nbytes = sizeof(data_t) * cfg->n;

	muggle_time_counter_init(&session_data->w_tc);
	muggle_time_counter_init(&session_data->r_tc);

	LOG_INFO("tcp session add into evloop, "
			 "local_ip=%s, local_port=%d, rcvbuf=%u, sndbuf=%u",
			 session_data->local_ip, session_data->local_port,
			 session_data->rcv_bufsize, session_data->snd_bufsize);

	// run snd datas
	muggle_socket_ctx_ref_retain(ctx);

	muggle_thread_create(&session_data->thread_snd, snd_data_thread_routine,
						 ctx);
}
void client_on_message(muggle_event_loop_t *evloop,
					   muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	session_data_t *session_data = get_session_info(ctx);

	int n = 0;
	char buf[65536];
	while ((n = muggle_socket_ctx_read(ctx, buf, sizeof(buf))) > 0) {
		session_data->total_rcv_nbytes += n;
	}

	if (session_data->total_rcv_nbytes == session_data->total_data_nbytes) {
		muggle_time_counter_end(&session_data->r_tc);
		muggle_thread_join(&session_data->thread_snd);

		int64_t elapsed_w =
			muggle_time_counter_interval_ns(&session_data->w_tc);
		int64_t elapsed_wr =
			muggle_time_counter_interval_ns(&session_data->r_tc);
		double elapsed_w_ms = elapsed_w / 1000000.0;
		double elapsed_wr_ms = elapsed_wr / 1000000.0;
		LOG_INFO(
			"elapsed time, w_start->w_end: %.3f ms, w_start->r_end: %.3f ms",
			elapsed_w_ms, elapsed_wr_ms);
	}
}
void client_on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	session_data_t *session_data = get_session_info(ctx);
	LOG_INFO("client session close, "
			 "remote_ip=%s, remote_port=%d",
			 session_data->remote_ip, session_data->remote_port);
}
void client_on_release(muggle_event_loop_t *evloop,
					   muggle_socket_context_t *ctx)
{
	MUGGLE_UNUSED(evloop);

	session_data_t *session_data = get_session_info(ctx);
	LOG_INFO("client session release, "
			 "remote_ip=%s, remote_port=%d",
			 session_data->remote_ip, session_data->remote_port);

	release_session_info(ctx);
}
