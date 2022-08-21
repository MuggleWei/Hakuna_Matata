#include "muggle/c/muggle_c.h"
#include "wolfssl/options.h"
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"

typedef struct conn_thread_args
{
	muggle_event_loop_t *evloop;
	const char *host;
	const char *port;
} conn_thread_args_t;

typedef struct ev_data
{
	WOLFSSL_CTX             *ssl_ctx;
	muggle_socket_context_t *socket_ctx;
	conn_thread_args_t      *conn_args;
} ev_data_t;

typedef struct socket_ctx_data
{
	WOLFSSL *ssl;
	int     ssl_established;  // is SSL established done
} socket_ctx_data_t;

muggle_thread_ret_t conn_thread_routine(void *p_args)
{
	conn_thread_args_t *args = (conn_thread_args_t*)p_args;

	while (1)
	{
		// create socket fd
		muggle_socket_t fd = muggle_tcp_connect(args->host, args->port, 3);
		if (fd == MUGGLE_INVALID_SOCKET)
		{
			LOG_ERROR("failed connect to: %s %s, wait retry", args->host, args->port);
			muggle_msleep(3000);
			continue;
		}

		// create socket context
		muggle_socket_context_t *ctx = (muggle_socket_context_t*)malloc(sizeof(muggle_socket_context_t));
		muggle_socket_ctx_init(ctx, fd, NULL, MUGGLE_SOCKET_CTX_TYPE_TCP_CLIENT);

		// add context to event loop
		muggle_socket_evloop_add_ctx(args->evloop, ctx);

		// exit connect thread
		break;
	}

	return 0;
}

void run_conn_thread(conn_thread_args_t *args)
{
	muggle_thread_t th;
	muggle_thread_create(&th, conn_thread_routine, args);
	muggle_thread_detach(&th);
}

WOLFSSL_CTX* gen_ssl_ctx(const char *ca_file)
{
	// WOLFSSL_METHOD *method = wolfTLS_client_method();
	// manual specify TLS version
	WOLFSSL_METHOD *method = wolfTLSv1_3_client_method();
	WOLFSSL_CTX *ctx = wolfSSL_CTX_new(method);
	if (ctx == NULL)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed wolfSSL_CTX_new");
		goto init_ssl_ctx_except;
	}

	int ret = 0;

	// load client certificates
	ret = wolfSSL_CTX_load_verify_locations(ctx, ca_file, NULL);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed wolfssl load ca file: %s", ca_file);
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
int ssl_connect(WOLFSSL *ssl)
{
	int ret = wolfSSL_connect(ssl);
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
		LOG_ERROR("failed ssl connect, err=%d", err);
		return -1;
	}
}

void do_ssl_connect(muggle_socket_context_t *ctx, socket_ctx_data_t *ctx_data)
{
	LOG_INFO("do ssl connect");

	WOLFSSL *ssl = ctx_data->ssl;
	int ret = ssl_connect(ssl);
	if (ret == 0)
	{
		LOG_INFO("ssl connect done");
		ctx_data->ssl_established = 1;
	}
	else if (ret == -1)
	{
		LOG_ERROR("failed connect ssl server side");
		muggle_socket_ctx_shutdown(ctx);
	}
}

void on_add_ctx(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	LOG_INFO("on add socket context into event loop");

	ev_data_t *ev_data = (ev_data_t*)muggle_evloop_get_data(evloop);

	// create socket ctx data
	socket_ctx_data_t *ctx_data = (socket_ctx_data_t*)malloc(sizeof(socket_ctx_data_t));
	memset(ctx_data, 0, sizeof(*ctx_data));

	// create WOLFSSL object
	WOLFSSL *ssl = wolfSSL_new(ev_data->ssl_ctx);
	if (ssl == NULL)
	{
		LOG_ERROR("failed new connect ssl");
		muggle_socket_ctx_shutdown(ctx);
		free(ctx_data);
		return;
	}
	ctx_data->ssl = ssl;

	// attach wolfSSL to the socket
	int ret = wolfSSL_set_fd(ssl, ctx->base.fd);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed set ssl fd");
		muggle_socket_ctx_shutdown(ctx);
		free(ctx_data);
		return;
	}
	ctx_data->ssl_established = 0;

	// store socket context data
	muggle_socket_ctx_set_data(ctx, ctx_data);

	// store socket context into event loop data
	ev_data->socket_ctx = ctx;

	// connect to ssl on the server side
	do_ssl_connect(ctx, ctx_data);
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
		do_ssl_connect(ctx, ctx_data);
		if (!ctx_data->ssl_established)
		{
			return;
		}
	}

	// read message
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
	} while(1);
}

void on_timer(muggle_event_loop_t *evloop)
{
	ev_data_t *ev_data = muggle_evloop_get_data(evloop);

	muggle_socket_context_t *ctx = ev_data->socket_ctx;
	if (ctx == NULL)
	{
		return;
	}

	socket_ctx_data_t *ctx_data = muggle_socket_ctx_get_data(ctx);
	if (ctx_data == NULL)
	{
		return;
	}

	if (!ctx_data->ssl_established)
	{
		return;
	}

	WOLFSSL *ssl = ctx_data->ssl;

	// get current ts
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	char buf[64];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%llu.%03d",
		(unsigned long long)ts.tv_sec, (int)(ts.tv_nsec / 1000000));

	LOG_INFO("on timer, send message %s", buf);
	
	// write to server
	size_t len = strlen(buf);
	if (wolfSSL_write(ssl, buf, len) != len)
	{
		LOG_ERROR("failed wolfssl write");
	}
}

void on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	LOG_WARNING("on disconnect");

	ev_data_t *ev_data = (ev_data_t*)muggle_evloop_get_data(evloop);
	ev_data->socket_ctx = NULL;

	// reconnect
	run_conn_thread(ev_data->conn_args);
}

void on_release(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx)
{
	LOG_INFO("on release");

	socket_ctx_data_t *ctx_data = muggle_socket_ctx_get_data(ctx);
	if (ctx_data == NULL)
	{
		return;
	}

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

	if (argc < 3)
	{
		LOG_ERROR("usage: %s host port", argv[0]);
		exit(EXIT_FAILURE);
	}

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
	ev_init_args.hints_max_fd = 4;
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
	muggle_socket_evloop_handle_set_timer_interval(&handle, 3 * 1000);
	muggle_socket_evloop_handle_set_cb_add_ctx(&handle, on_add_ctx);
	muggle_socket_evloop_handle_set_cb_msg(&handle, on_message);
	muggle_socket_evloop_handle_set_cb_timer(&handle, on_timer);
	muggle_socket_evloop_handle_set_cb_close(&handle, on_close);
	muggle_socket_evloop_handle_set_cb_release(&handle, on_release);
	muggle_socket_evloop_handle_attach(&handle, evloop);
	LOG_INFO("socket handle attached event loop");

	// create tcp connect socket
	conn_thread_args_t conn_args;
	memset(&conn_args, 0, sizeof(conn_args));
	conn_args.evloop = evloop;
	conn_args.host = argv[1];
	conn_args.port = argv[2];
	run_conn_thread(&conn_args);

	// create ssl context
	const char *ca_file = "certs/ca.crt";
	WOLFSSL_CTX *ssl_ctx = gen_ssl_ctx(ca_file);
	if (ssl_ctx == NULL)
	{
		LOG_ERROR("failed generate ssl ctx");
		exit(EXIT_FAILURE);
	}

	// set event loop data
	ev_data_t ev_data;
	memset(&ev_data, 0, sizeof(ev_data));
	ev_data.ssl_ctx = ssl_ctx;
	ev_data.conn_args = &conn_args;
	muggle_evloop_set_data(evloop, &ev_data);

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
