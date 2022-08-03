#include "muggle/c/muggle_c.h"
#include "wolfssl/ssl.h"

typedef struct user_ev_data
{
	WOLFSSL_CTX *ctx;
	muggle_socket_peer_t *peer;
} user_ev_data_t;

typedef struct user_peer_data
{
	WOLFSSL *ssl;
	int     ssl_established;  // is SSL established done
} user_peer_data_t;

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

void on_connect(struct muggle_socket_event *ev, struct muggle_socket_peer *listen_peer, struct muggle_socket_peer *peer)
{
	LOG_INFO("on connect");

	user_ev_data_t *ev_data = (user_ev_data_t*)ev->datas;

	// create peer data
	user_peer_data_t *peer_data = (user_peer_data_t*)malloc(sizeof(user_peer_data_t));
	if (peer_data == NULL)
	{
		LOG_ERROR("failed malloc peer data");
		muggle_socket_peer_close(peer);
		muggle_socket_event_loop_exit(ev);
		return;
	}
	peer->data = (void*)peer_data;
	memset(peer_data, 0, sizeof(*peer_data));

	// create WOLFSSL object
	WOLFSSL *ssl = wolfSSL_new(ev_data->ctx);
	if (ssl == NULL)
	{
		LOG_ERROR("failed new connect ssl");
		muggle_socket_peer_close(peer);
		muggle_socket_event_loop_exit(ev);
		return;
	}
	peer_data->ssl = ssl;

	// attach wolfSSL to the socket
	int ret = wolfSSL_set_fd(ssl, peer->fd);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed set ssl fd");
		muggle_socket_peer_close(peer);
		muggle_socket_event_loop_exit(ev);
		return;
	}

	// connect to wolfSSL on the server side
	peer_data->ssl_established = 0;
	ret = ssl_connect(ssl);
	if (ret == 0)
	{
		LOG_INFO("ssl connect done");
		peer_data->ssl_established = 1;
	}
	else if (ret == -1)
	{
		LOG_ERROR("failed connect ssl server side");
		muggle_socket_peer_close(peer);
		muggle_socket_event_loop_exit(ev);
		return;
	}

	// set client peer
	ev_data->peer = peer;
}

void on_error(struct muggle_socket_event *ev, struct muggle_socket_peer *peer)
{
	char straddr[MUGGLE_SOCKET_ADDR_STRLEN];
	if (peer->addr_len == 0 ||
		muggle_socket_ntop((struct sockaddr*)&peer->addr, straddr, MUGGLE_SOCKET_ADDR_STRLEN, 0) == NULL)
	{
		snprintf(straddr, MUGGLE_SOCKET_ADDR_STRLEN, "?:?");
	}
	LOG_ERROR("on disconnect, %s", straddr);

}

void on_close(struct muggle_socket_event *ev, struct muggle_socket_peer *peer)
{
	char straddr[MUGGLE_SOCKET_ADDR_STRLEN];
	if (peer->addr_len == 0 ||
		muggle_socket_ntop((struct sockaddr*)&peer->addr, straddr, MUGGLE_SOCKET_ADDR_STRLEN, 0) == NULL)
	{
		snprintf(straddr, MUGGLE_SOCKET_ADDR_STRLEN, "?:?");
	}
	LOG_ERROR("on close, %s", straddr);

	// free user data
	user_peer_data_t *peer_data = peer->data;
	if (peer_data)
	{
		if (peer_data->ssl)
		{
			wolfSSL_free(peer_data->ssl);
		}

		free(peer_data);
		peer->data = NULL;
	}

	// set ev_data->peer to NULL
	user_ev_data_t *ev_data = (user_ev_data_t*)ev->datas;
	if (ev_data->peer == NULL)
	{
		return;
	}

	// exit event loop
	muggle_socket_event_loop_exit(ev);
}

void on_message(struct muggle_socket_event *ev, struct muggle_socket_peer *peer)
{
	user_peer_data_t *peer_data = peer->data;
	if (peer_data == NULL)
	{
		return;
	}
	WOLFSSL *ssl = peer_data->ssl;

	// ensure ssl connect done
	if (!peer_data->ssl_established)
	{
		int ret = ssl_connect(ssl);
		if (ret == 0)
		{
			LOG_INFO("ssl connect done");
			peer_data->ssl_established = 1;
		}
		else if (ret == -1)
		{
			LOG_ERROR("failed connect ssl server side");
			muggle_socket_peer_close(peer);
			muggle_socket_event_loop_exit(ev);
			return;
		}
		else
		{
			// wait ssl connect done
			LOG_INFO("wait for ssl connect done");
			return;
		}
	}

	// read message
	char buf[512];
	memset(buf, 0, sizeof(buf));
	do {
		if (wolfSSL_read(ssl, buf, sizeof(buf)-1) == -1)
		{
			if (MUGGLE_SOCKET_LAST_ERRNO != MUGGLE_SYS_ERRNO_WOULDBLOCK)
			{
				LOG_ERROR("failed wolfssl read");
				muggle_socket_peer_close(peer);
			}
			break;
		}

		LOG_INFO("on message: %s", buf);
	} while(1);
}

void on_timer(struct muggle_socket_event *ev)
{
	// get client peer
	user_ev_data_t *ev_data = (user_ev_data_t*)ev->datas;
	muggle_socket_peer_t *peer = ev_data->peer;
	if (peer == NULL)
	{
		return;
	}

	user_peer_data_t *peer_data = peer->data;
	if (peer_data == NULL)
	{
		return;
	}
	WOLFSSL *ssl = peer_data->ssl;

	if (!peer_data->ssl_established)
	{
		// LOG_INFO("on timer, wait ssl connect done");
		return;
	}

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
		muggle_socket_peer_close(peer);
	}
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

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// // enable wolf debug message
	// wolfSSL_Debugging_ON();

	// initialize socket lib
	muggle_socket_lib_init();

	if (argc < 3)
	{
		LOG_ERROR("usage: %s host port", argv[0]);
		exit(EXIT_FAILURE);
	}

	// create client socket
	const char *host = argv[1];
	const char *serv = argv[2];

	// init ssl
	wolfSSL_Init();

	const char *ca_file = "certs/ca.crt";
	WOLFSSL_CTX *ctx = gen_ssl_ctx(ca_file);
	if (ctx == NULL)
	{
		LOG_ERROR("failed generate ssl ctx");
		exit(EXIT_FAILURE);
	}

	user_ev_data_t ev_data;
	memset(&ev_data, 0, sizeof(ev_data));
	ev_data.ctx = ctx;

	// run client event loop
	muggle_socket_peer_t *connect_peer = NULL;
	int connect_timeout = 3;
	while (1)
	{
		muggle_socket_peer_t peer;
		if (muggle_tcp_connect(host, serv, connect_timeout, &peer) == MUGGLE_INVALID_SOCKET)
		{
			MUGGLE_LOG_ERROR("failed connect %s:%s", host, serv);
			muggle_msleep(3000);
			continue;
		}

		// fill up event loop input arguments
		muggle_socket_event_init_arg_t ev_init_arg;
		memset(&ev_init_arg, 0, sizeof(ev_init_arg));
		ev_init_arg.hints_max_peer = 1024;
		ev_init_arg.cnt_peer = 1;
		ev_init_arg.peers = &peer;
		ev_init_arg.p_peers = &connect_peer;
		ev_init_arg.timeout_ms = 1000;
		ev_init_arg.datas = &ev_data;
		ev_init_arg.on_connect = NULL;
		ev_init_arg.on_error = on_error;
		ev_init_arg.on_close = on_close;
		ev_init_arg.on_message = on_message;
		ev_init_arg.on_timer = on_timer;

		// event loop init
		muggle_socket_event_t ev;
		if (muggle_socket_event_init(&ev_init_arg, &ev) != 0)
		{
			MUGGLE_LOG_ERROR("failed init socket event");
			exit(EXIT_FAILURE);
		}

		// on client connection
		on_connect(&ev, NULL, connect_peer);

		// event loop
		muggle_socket_event_loop(&ev);

		muggle_msleep(3000);
	}

	// cleanup ssl
	wolfSSL_CTX_free(ctx);
	wolfSSL_Cleanup();

	return 0;
}
