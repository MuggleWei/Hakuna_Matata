#include "client.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "ssl_session.h"
#include <muggle/c/net/socket_context.h>

typedef struct client_evloop_data {
	SSL_CTX *ssl_ctx;
	muggle_socket_context_t *ctx;
} client_evloop_data_t;

static muggle_socket_context_t *tcp_connect(const char *host, const char *port)
{
	muggle_socket_t fd = muggle_tcp_connect(host, port, 3);
	if (fd == MUGGLE_INVALID_SOCKET) {
		LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed listen");
		return NULL;
	}
	LOG_DEBUG("success connect to %s:%s", host, port);

	muggle_socket_context_t *ctx =
		(muggle_socket_context_t *)malloc(sizeof(muggle_socket_context_t));
	if (ctx == NULL) {
		LOG_ERROR("failed allocate memory space for listen socket ctx");
		muggle_socket_close(fd);
		return NULL;
	}

	muggle_socket_ctx_init(ctx, fd, NULL, MUGGLE_SOCKET_CTX_TYPE_TCP_CLIENT);

	return ctx;
}

static SSL_CTX *new_client_ssl_ctx()
{
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if (ctx == NULL) {
		LOG_ERROR("failed create SSL context, err: %s",
				  ERR_reason_error_string(ERR_get_error()));
		return NULL;
	}

	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

	const char *crt_filepath = "certs/server.crt";
	if (!SSL_CTX_load_verify_locations(ctx, crt_filepath, NULL)) {
		LOG_ERROR("failed load verify file: "
				  "crt_filepath=%s, err=%s",
				  crt_filepath, ERR_reason_error_string(ERR_get_error()));
		SSL_CTX_free(ctx);
		return NULL;
	}

	LOG_DEBUG("success new ssl context");

	return ctx;
}

static bool ssl_session_connect(ssl_session_t *session)
{
	if (session->status == 1) {
		return true;
	}

	int ret = SSL_connect(session->ssl);
	LOG_DEBUG("ssl session connect, return: %d", ret);
	if (ret == 0) {
		// FATAL ERROR
		int err_code = SSL_get_error(session->ssl, ret);
		OUTPUT_SSL_SESSION_ERROR(session, err_code);
		return false;
	} else if (ret == 1) {
		// SUCCESS
		session->status = 1;
		LOG_DEBUG("ssl connect success: remote_addr=%s", session->remote_addr);
	} else if (ret < 0) {
		int err_code = SSL_get_error(session->ssl, ret);
		if (err_code == SSL_ERROR_WANT_READ) {
			// wait for data to be read
			return true;
		} else {
			OUTPUT_SSL_SESSION_ERROR(session, err_code);
			return false;
		}
	} else {
		// FATAL ERROR
		LOG_ERROR("failed ssl connect: remote_addr=%s", session->remote_addr);
		return false;
	}

	return true;
}

bool run_ssl_client(const char *host, const char *port)
{
	muggle_event_loop_t *evloop = NULL;
	muggle_socket_evloop_handle_t *handle = NULL;
	SSL_CTX *ssl_ctx = NULL;
	muggle_socket_context_t *conn_ctx = NULL;
	client_evloop_data_t *user_data = NULL;
	bool ret = true;

	// init event loop
	muggle_event_loop_init_args_t ev_init_args;
	memset(&ev_init_args, 0, sizeof(ev_init_args));
	ev_init_args.evloop_type = MUGGLE_EVLOOP_TYPE_NULL;
	ev_init_args.hints_max_fd = 64;
	ev_init_args.use_mem_pool = 0;

	evloop = muggle_evloop_new(&ev_init_args);
	if (evloop == NULL) {
		LOG_ERROR("failed new event loop");
		ret = false;
		goto cleanup;
	}
	LOG_DEBUG("success new event loop");

	// init socket event loop handle
	handle = (muggle_socket_evloop_handle_t *)malloc(
		sizeof(muggle_socket_evloop_handle_t));
	if (handle == NULL) {
		LOG_ERROR("failed allocate memory space for evloop handle");
		ret = false;
		goto cleanup;
	}
	if (muggle_socket_evloop_handle_init(handle) != 0) {
		LOG_ERROR("failed init evloop handle");
		free(handle);
		handle = NULL;
		ret = false;
		goto cleanup;
	}

	muggle_socket_evloop_handle_set_cb_add_ctx(handle, ssl_client_on_add_ctx);
	muggle_socket_evloop_handle_set_cb_msg(handle, ssl_client_on_message);
	muggle_socket_evloop_handle_set_cb_close(handle, ssl_client_on_close);
	muggle_socket_evloop_handle_set_cb_release(handle, ssl_client_on_release);
	muggle_socket_evloop_handle_set_cb_timer(handle, ssl_client_on_timer);
	muggle_socket_evloop_handle_set_timer_interval(handle, 3 * 1000);
	muggle_socket_evloop_handle_attach(handle, evloop);
	LOG_DEBUG("socket handle attached event loop");

	// prepare ssl
	ssl_ctx = new_client_ssl_ctx();
	if (ssl_ctx == NULL) {
		LOG_ERROR("failed new client ssl context");
		ret = false;
		goto cleanup;
	}

	// tcp connect
	conn_ctx = tcp_connect(host, port);
	if (conn_ctx == NULL) {
		LOG_ERROR("failed connect to %s:%s", host, port);
		ret = false;
		goto cleanup;
	}
	muggle_socket_evloop_add_ctx(evloop, conn_ctx);

	// set evloop user data
	user_data = (client_evloop_data_t *)malloc(sizeof(client_evloop_data_t));
	if (user_data == NULL) {
		LOG_ERROR("failed allocate memory space for evloop data");
		ret = false;
		goto cleanup;
	}
	memset(user_data, 0, sizeof(*user_data));
	user_data->ssl_ctx = ssl_ctx;

	muggle_evloop_set_data(evloop, user_data);

	// run event loop
	muggle_evloop_run(evloop);

cleanup:
	if (user_data) {
		free(user_data);
		user_data = NULL;
	}

	if (conn_ctx) {
		free(conn_ctx);
		conn_ctx = NULL;
	}

	if (ssl_ctx) {
		SSL_CTX_free(ssl_ctx);
		ssl_ctx = NULL;
	}

	if (handle) {
		muggle_socket_evloop_handle_destroy(handle);
		free(handle);
		handle = NULL;
	}

	if (evloop) {
		muggle_evloop_delete(evloop);
		evloop = NULL;
	}

	return ret;
}

void ssl_client_on_add_ctx(muggle_event_loop_t *evloop,
						   muggle_socket_context_t *ctx)
{
	client_evloop_data_t *data =
		(client_evloop_data_t *)muggle_evloop_get_data(evloop);
	ssl_session_t *session = ssl_session_new(ctx, data->ssl_ctx);
	if (session) {
		muggle_socket_ctx_set_data(ctx, session);
		data->ctx = ctx;

		LOG_DEBUG("on add context");

		char remote_ip[MUGGLE_SOCKET_ADDR_STRLEN];
		muggle_socket_remote_addr(muggle_socket_ctx_get_fd(ctx), remote_ip,
								  sizeof(remote_ip), 1);
		// set host for SNI
		SSL_set_tlsext_host_name(session->ssl, remote_ip);
		// configure server hostname check
		SSL_set1_host(session->ssl, remote_ip);

		if (!ssl_session_connect(session)) {
			LOG_ERROR("failed ssl session connect: remote_addr=%s",
					  session->remote_addr);
			muggle_socket_ctx_close(ctx);
			return;
		}
	} else {
		LOG_ERROR("failed new ssl session");
	}
}

void ssl_client_on_message(muggle_event_loop_t *evloop,
						   muggle_socket_context_t *ctx)
{
	ssl_session_t *session = muggle_socket_ctx_get_data(ctx);
	if (session == NULL) {
		LOG_FATAL("failed get ssl session");
		return;
	}

	if (session->status == 0) {
		if (!ssl_session_connect(session)) {
			LOG_ERROR("failed ssl session connect: remote_addr=%s",
					  session->remote_addr);
			muggle_socket_ctx_close(ctx);
			return;
		}
		if (session->status == 0) {
			return;
		}
	}

	// echo loop
	while (true) {
		char buf[1024];
		int numbytes = SSL_read(session->ssl, buf, sizeof(buf) - 1);
		if (numbytes <= 0) {
			int err_code = SSL_get_error(session->ssl, numbytes);
			if (err_code != SSL_ERROR_WANT_READ) {
				OUTPUT_SSL_SESSION_ERROR(session, err_code);
				muggle_socket_ctx_close(ctx);
			}
			break;
		} else {
			buf[numbytes] = '\0';
			LOG_DEBUG("recv message: %s", buf);
		}
	}
}

void ssl_client_on_close(muggle_event_loop_t *evloop,
						 muggle_socket_context_t *ctx)
{
	ssl_session_t *session = muggle_socket_ctx_get_data(ctx);
	if (session == NULL) {
		LOG_FATAL("failed get ssl session");
		return;
	}

	LOG_DEBUG("on close: remote_addr=%s", session->remote_addr);

	client_evloop_data_t *data =
		(client_evloop_data_t *)muggle_evloop_get_data(evloop);
	if (data->ctx == ctx) {
		data->ctx = NULL;
	}
}

void ssl_client_on_release(muggle_event_loop_t *evloop,
						   muggle_socket_context_t *ctx)
{
	ssl_session_t *session = muggle_socket_ctx_get_data(ctx);
	if (session == NULL) {
		LOG_FATAL("failed get ssl session");
		return;
	}
	LOG_DEBUG("on release: remote_addr=%s", session->remote_addr);

	ssl_session_delete(session);
	muggle_socket_ctx_set_data(ctx, NULL);
}

void ssl_client_on_timer(muggle_event_loop_t *evloop)
{
	MUGGLE_UNUSED(evloop);

	client_evloop_data_t *data =
		(client_evloop_data_t *)muggle_evloop_get_data(evloop);
	if (data->ctx == NULL) {
		return;
	}
	ssl_session_t *session = muggle_socket_ctx_get_data(data->ctx);

	if (session->status == 0) {
		if (!ssl_session_connect(session)) {
			LOG_ERROR("failed ssl session connect: remote_addr=%s",
					  session->remote_addr);
			muggle_socket_ctx_close(data->ctx);
			return;
		}
		if (session->status == 0) {
			return;
		}
	}

	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	time_t sec = (time_t)ts.tv_sec;
	struct tm t;
	localtime_r(&sec, &t);

	char buf[128];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%d-%02d-%02dT%02d:%02d:%02d.%09ld",
			 t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
			 t.tm_sec, (long)ts.tv_nsec);

	int numbytes = SSL_write(session->ssl, buf, strlen(buf) + 1);
	if (numbytes <= 0) {
		int err_code = SSL_get_error(session->ssl, numbytes);
		if (err_code != SSL_ERROR_WANT_READ) {
			OUTPUT_SSL_SESSION_ERROR(session, err_code);
			muggle_socket_ctx_close(data->ctx);
		}
	} else {
		LOG_DEBUG("send message: %s", buf);
	}
}
