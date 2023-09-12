#include "server.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "ssl_session.h"

typedef struct server_evloop_data {
	SSL_CTX *ssl_ctx;
} server_evloop_data_t;

static muggle_socket_context_t *tcp_listen(const char *host, const char *port)
{
	muggle_socket_t fd = muggle_tcp_listen(host, port, 512);
	if (fd == MUGGLE_INVALID_SOCKET) {
		LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed listen");
		return NULL;
	}
	LOG_DEBUG("success listen on %s:%s", host, port);

	muggle_socket_context_t *ctx =
		(muggle_socket_context_t *)malloc(sizeof(muggle_socket_context_t));
	if (ctx == NULL) {
		LOG_ERROR("failed allocate memory space for listen socket ctx");
		muggle_socket_close(fd);
		return NULL;
	}

	muggle_socket_ctx_init(ctx, fd, NULL, MUGGLE_SOCKET_CTX_TYPE_TCP_LISTEN);

	return ctx;
}

static SSL_CTX *new_server_ssl_ctx()
{
	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	if (ctx == NULL) {
		LOG_ERROR("failed create SSL context, err: %s",
				  ERR_reason_error_string(ERR_get_error()));
		return NULL;
	}

	const char *crt_filepath = "certs/server.crt";
	const char *key_filepath = "certs/server.key";
	if (SSL_CTX_use_certificate_chain_file(ctx, crt_filepath) <= 0) {
		LOG_ERROR("failed set certificate chain file: "
				  "crt_filepath=%s, err=%s",
				  crt_filepath, ERR_reason_error_string(ERR_get_error()));
		SSL_CTX_free(ctx);
		return NULL;
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, key_filepath, SSL_FILETYPE_PEM) <= 0) {
		LOG_ERROR("failed set private key file: "
				  "key_filepath=%s, err=%s",
				  key_filepath, ERR_reason_error_string(ERR_get_error()));
		SSL_CTX_free(ctx);
		return NULL;
	}

	LOG_DEBUG("success new ssl context");

	return ctx;
}

static bool ssl_session_accept(ssl_session_t *session)
{
	if (session->status == 1) {
		return true;
	}

	int ret = SSL_accept(session->ssl);
	if (ret == 0) {
		// FATAL ERROR
		int err_code = SSL_get_error(session->ssl, ret);
		OUTPUT_SSL_SESSION_ERROR(session, err_code);
		return false;
	} else if (ret == 1) {
		// SUCCESS
		session->status = 1;
		LOG_DEBUG("ssl accept success: remote_addr=%s", session->remote_addr);
	} else if (ret < 0) {
		int err_code = SSL_get_error(session->ssl, ret);
		if (err_code == SSL_ERROR_WANT_READ) {
			// wait for data to be read
			LOG_DEBUG("ssl session establishing");
			return true;
		} else {
			OUTPUT_SSL_SESSION_ERROR(session, err_code);
			return false;
		}
	} else {
		// FATAL ERROR
		LOG_ERROR("failed ssl accept: remote_addr=%s", session->remote_addr);
		return false;
	}

	return true;
}

bool run_ssl_server(const char *host, const char *port)
{
	muggle_event_loop_t *evloop = NULL;
	muggle_socket_evloop_handle_t *handle = NULL;
	SSL_CTX *ssl_ctx = NULL;
	muggle_socket_context_t *listen_ctx = NULL;
	server_evloop_data_t *user_data = NULL;
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
	muggle_socket_evloop_handle_set_cb_conn(handle, ssl_serv_on_connect);
	muggle_socket_evloop_handle_set_cb_msg(handle, ssl_serv_on_message);
	muggle_socket_evloop_handle_set_cb_close(handle, ssl_serv_on_close);
	muggle_socket_evloop_handle_set_cb_release(handle, ssl_serv_on_release);
	muggle_socket_evloop_handle_set_cb_timer(handle, ssl_serv_on_timer);
	muggle_socket_evloop_handle_set_timer_interval(handle, 3 * 1000);
	muggle_socket_evloop_handle_attach(handle, evloop);
	LOG_DEBUG("socket handle attached event loop");

	// prepare ssl
	ssl_ctx = new_server_ssl_ctx();
	if (ssl_ctx == NULL) {
		LOG_ERROR("failed new server ssl context");
		ret = false;
		goto cleanup;
	}

	// tcp listen
	listen_ctx = tcp_listen(host, port);
	if (listen_ctx == NULL) {
		LOG_ERROR("failed listen on %s:%s", host, port);
		ret = false;
		goto cleanup;
	}
	muggle_socket_evloop_add_ctx(evloop, listen_ctx);

	// set evloop user data
	user_data = (server_evloop_data_t *)malloc(sizeof(server_evloop_data_t));
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

	// NOTE:
	//   listen context already add into eventloop, don't need free it manaual
	// if (listen_ctx) {
	//     free(listen_ctx);
	//     listen_ctx = NULL;
	// }

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

void ssl_serv_on_connect(muggle_event_loop_t *evloop,
						 muggle_socket_context_t *ctx)
{
	server_evloop_data_t *data =
		(server_evloop_data_t *)muggle_evloop_get_data(evloop);
	ssl_session_t *session = ssl_session_new(ctx, data->ssl_ctx);
	if (session) {
		muggle_socket_ctx_set_data(ctx, session);

		LOG_DEBUG("new connection: remote_addr=%s", session->remote_addr);
		if (!ssl_session_accept(session)) {
			LOG_ERROR("failed ssl session accept: remote_addr=%s",
					  session->remote_addr);
			muggle_socket_ctx_close(ctx);
			return;
		}
	} else {
		LOG_ERROR("failed new ssl session");
	}
}

void ssl_serv_on_message(muggle_event_loop_t *evloop,
						 muggle_socket_context_t *ctx)
{
	ssl_session_t *session = muggle_socket_ctx_get_data(ctx);
	if (session == NULL) {
		LOG_FATAL("failed get ssl session");
		return;
	}

	if (session->status == 0) {
		if (!ssl_session_accept(session)) {
			LOG_ERROR("failed ssl session accept: remote_addr=%s",
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
		}

		buf[numbytes] = '\0';
		LOG_DEBUG("recv message: %s", buf);

		numbytes = SSL_write(session->ssl, buf, numbytes);
		if (numbytes <= 0) {
			int err_code = SSL_get_error(session->ssl, numbytes);
			OUTPUT_SSL_SESSION_ERROR(session, err_code);
			muggle_socket_ctx_close(ctx);
			break;
		}
	}
}

void ssl_serv_on_close(muggle_event_loop_t *evloop,
					   muggle_socket_context_t *ctx)
{
	if (ctx->sock_type == MUGGLE_SOCKET_CTX_TYPE_TCP_LISTEN) {
		return;
	}

	ssl_session_t *session = muggle_socket_ctx_get_data(ctx);
	if (session == NULL) {
		LOG_FATAL("failed get ssl session");
		return;
	}

	LOG_DEBUG("on close: remote_addr=%s", session->remote_addr);
}

void ssl_serv_on_release(muggle_event_loop_t *evloop,
						 muggle_socket_context_t *ctx)
{
	if (ctx->sock_type == MUGGLE_SOCKET_CTX_TYPE_TCP_LISTEN) {
		return;
	}

	ssl_session_t *session = muggle_socket_ctx_get_data(ctx);
	if (session == NULL) {
		LOG_FATAL("failed get ssl session");
		return;
	}
	LOG_DEBUG("on release: remote_addr=%s", session->remote_addr);

	ssl_session_delete(session);
	muggle_socket_ctx_set_data(ctx, NULL);
}

void ssl_serv_on_timer(muggle_event_loop_t *evloop)
{
	MUGGLE_UNUSED(evloop);
}
