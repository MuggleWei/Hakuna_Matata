#include "muggle/c/muggle_c.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"

typedef struct evloop_data
{
	WOLFSSL_CTX *ssl_ctx;
} evloop_data_t;

typedef struct socket_ctx_data
{
	char remote_addr[MUGGLE_SOCKET_ADDR_STRLEN];
	WOLFSSL *ssl;
	int     ssl_established;  // is SSL established done
} socket_ctx_data_t;

WOLFSSL_CTX* gen_ssl_ctx(const char *cert_file, const char *key_file)
{
	// WOLFSSL_METHOD *method = wolfTLS_server_method();
	// manual specify TLS version
	WOLFSSL_METHOD *method = wolfTLSv1_3_server_method();
	WOLFSSL_CTX *ctx = wolfSSL_CTX_new(method);
	if (ctx == NULL)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed wolfSSL_CTX_new");
		goto init_ssl_ctx_except;
	}

	int ret = 0;

	// load certificateds
	ret = wolfSSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed wolfssl load cert file: %s", cert_file);
		goto init_ssl_ctx_except;
	}

	// load key
	ret = wolfSSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed wolfssl load key file: %s", cert_file);
		goto init_ssl_ctx_except;
	}

	return ctx;

init_ssl_ctx_except:
	if (ctx != NULL)
	{
		wolfSSL_CTX_free(ctx);
	}

	return NULL;
}

/**
 * @brief  try established ssl
 *
 * @param ssl
 *
 * @return 
 *     -1: failure
 *     0: success
 *     otherwise: wait to established done
 */
int ssl_accept(WOLFSSL *ssl)
{
	int ret = wolfSSL_accept(ssl);
	int err = wolfSSL_get_error(ssl, ret);
	if (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE)
	{
		return 1;
	}

	if (ret == SSL_SUCCESS)
	{
		return 0;
	}
	else
	{
		LOG_ERROR("failed ssl accept, err=%d", err);
		return -1;
	}
}

void do_ssl_accept(muggle_socket_context_t *ctx, socket_ctx_data_t *ctx_data)
{
	LOG_INFO("do ssl accept: %s", ctx_data->remote_addr);

	WOLFSSL *ssl = ctx_data->ssl;
	int ret = ssl_accept(ssl);
	if (ret == 0)
	{
		LOG_INFO("ssl accept done");
		ctx_data->ssl_established = 1;
	}
	else if (ret == -1)
	{
		LOG_ERROR("failed ssl accept: %s", ctx_data->remote_addr);
		muggle_socket_ctx_shutdown(ctx);
	}
	else
	{
		LOG_INFO("ssl accept established not finish yet");
	}
}


muggle_socket_context_t* tcp_listen(const char *host, const char *serv)
{
	// tcp listen
	muggle_socket_t listen_fd = MUGGLE_INVALID_SOCKET;
	do {
		listen_fd = muggle_tcp_listen(host, serv, 512);
		if (listen_fd == MUGGLE_INVALID_SOCKET)
		{
			LOG_ERROR("failed create tcp listen for %s:%s", host, serv);
			muggle_msleep(3000);
		}
	} while(listen_fd == MUGGLE_INVALID_SOCKET);
	LOG_INFO("success listen %s %s", host, serv);

	// new context
	muggle_socket_context_t *listen_ctx =
		(muggle_socket_context_t*)malloc(sizeof(muggle_socket_context_t));
	if (muggle_socket_ctx_init(listen_ctx, listen_fd, NULL, MUGGLE_SOCKET_CTX_TYPE_TCP_LISTEN) != 0)
	{
		muggle_socket_close(listen_fd);
		LOG_ERROR("failed create socket context");
		return NULL;
	}

	return listen_ctx;
}

void on_connect(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	// get remote address
	socket_ctx_data_t *ctx_data = (socket_ctx_data_t*)malloc(sizeof(socket_ctx_data_t));
	memset(ctx_data, 0, sizeof(*ctx_data));
	muggle_socket_remote_addr(ctx->base.fd, ctx_data->remote_addr, sizeof(ctx_data->remote_addr), 0);

	LOG_INFO("on connect: %s", ctx_data->remote_addr);

	// create ssl object
	evloop_data_t *ev_data = muggle_evloop_get_data(evloop);
	WOLFSSL *ssl = wolfSSL_new(ev_data->ssl_ctx);
	if (ssl == NULL)
	{
		LOG_ERROR("failed new connect ssl: %s", ctx_data->remote_addr);
		muggle_socket_ctx_shutdown(ctx);
		free(ctx_data);
		return;
	}
	ctx_data->ssl = ssl;

	// attach ssl to the socket
	int ret = wolfSSL_set_fd(ssl, ctx->base.fd);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed set ssl fd: %s", ctx_data->remote_addr);
		muggle_socket_ctx_shutdown(ctx);
		free(ctx_data);
		return;
	}
	ctx_data->ssl_established = 0;

	// store socket context data
	muggle_socket_ctx_set_data(ctx, ctx_data);
}

void on_message(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	socket_ctx_data_t *ctx_data = muggle_socket_ctx_get_data(ctx);
	if (ctx_data == NULL)
	{
		MUGGLE_ASSERT_MSG(ctx_data != NULL, "socket ctx data is NULL!");
		return;
	}

	// ensure ssl accept done
	if (!ctx_data->ssl_established)
	{
		do_ssl_accept(ctx, ctx_data);
		if (!ctx_data->ssl_established)
		{
			return;
		}
	}

	// echo message
	WOLFSSL *ssl = ctx_data->ssl;
	char buf[512];
	memset(buf, 0, sizeof(buf));
	do {
		if (wolfSSL_read(ssl, buf, sizeof(buf)-1) == -1)
		{
			if (MUGGLE_SOCKET_LAST_ERRNO != MUGGLE_SYS_ERRNO_WOULDBLOCK)
			{
				LOG_ERROR("failed wolfssl read");
				muggle_socket_ctx_shutdown(ctx);
			}
			break;
		}

		LOG_INFO("on message: %s", buf);

		size_t len = strlen(buf);
		if (wolfSSL_write(ssl, buf, len) != len)
		{
			LOG_ERROR("failed wolfssl write");
			muggle_socket_ctx_shutdown(ctx);
			break;
		}
	} while(1);
}

void on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	socket_ctx_data_t *ctx_data = muggle_socket_ctx_get_data(ctx);
	if (ctx_data == NULL)
	{
		MUGGLE_ASSERT_MSG(ctx_data != NULL, "socket ctx data is NULL!");
		return;
	}

	LOG_INFO("on close: %s", ctx_data->remote_addr);
}

void on_release(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	socket_ctx_data_t *ctx_data = muggle_socket_ctx_get_data(ctx);
	if (ctx_data == NULL)
	{
		return;
	}

	LOG_INFO("on release: %s", ctx_data->remote_addr);

	if (ctx_data->ssl)
	{
		wolfSSL_free(ctx_data->ssl);
	}
	free(ctx_data);

	muggle_socket_ctx_set_data(ctx, NULL);
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// parse input arguments
	if (argc < 3)
	{
		LOG_ERROR("usage: %s host port", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char *host = argv[1];
	const char *serv = argv[2];

	// // enable wolf debug message
	// wolfSSL_Debugging_ON();

	// initialize socket lib
	muggle_socket_lib_init();

	// init ssl
	wolfSSL_Init();

	// init event loop
	muggle_event_loop_init_args_t ev_init_args;
	memset(&ev_init_args, 0, sizeof(ev_init_args));
	ev_init_args.evloop_type = MUGGLE_EVLOOP_TYPE_NULL;
	ev_init_args.hints_max_fd = 32;
	ev_init_args.use_mem_pool = 0;

	muggle_event_loop_t *evloop = muggle_evloop_new(&ev_init_args);
	if (evloop == NULL)
	{
		LOG_ERROR("failed new event loop");
		exit(EXIT_FAILURE);
	}
	LOG_INFO("success new event loop");

	// init socket event loop handle
	muggle_socket_evloop_handle_t handle;
	muggle_socket_evloop_handle_init(&handle);
	muggle_socket_evloop_handle_set_cb_conn(&handle, on_connect);
	muggle_socket_evloop_handle_set_cb_msg(&handle, on_message);
	muggle_socket_evloop_handle_set_cb_close(&handle, on_close);
	muggle_socket_evloop_handle_set_cb_release(&handle, on_release);
	muggle_socket_evloop_handle_attach(&handle, evloop);
	LOG_INFO("socket handle attached event loop");

	// create tcp listen socket and add into socket event loop
	muggle_socket_context_t *tcp_listen_ctx = tcp_listen(host, serv);
	muggle_socket_evloop_add_ctx(evloop, tcp_listen_ctx);

	// create ssl context
	const char *cert_file = "certs/server.crt";
	const char *key_file = "certs/server.key";
	WOLFSSL_CTX *ssl_ctx = gen_ssl_ctx(cert_file, key_file);
	if (ssl_ctx == NULL)
	{
		LOG_ERROR("failed generate ssl ctx");
		exit(EXIT_FAILURE);
	}

	// set event loop data
	evloop_data_t data;
	memset(&data, 0, sizeof(data));
	data.ssl_ctx = ssl_ctx;
	muggle_evloop_set_data(evloop, &data);

	// run event loop
	LOG_INFO("run event loop");
	muggle_evloop_run(evloop);

	// cleanup ssl
	wolfSSL_CTX_free(ssl_ctx);
	wolfSSL_Cleanup();

	// destroy socket event loop handle
	muggle_socket_evloop_handle_destroy(&handle);

	// delete event loop
	muggle_evloop_delete(evloop);

	return 0;
}
