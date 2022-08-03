#include "muggle/c/muggle_c.h"
#include "wolfssl/ssl.h"

typedef struct user_ev_data
{
	WOLFSSL_CTX *ctx;
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
		return;
	}
	peer_data->ssl = ssl;

	// attach wolfSSL to the socket
	int ret = wolfSSL_set_fd(ssl, peer->fd);
	if (ret != SSL_SUCCESS)
	{
		LOG_ERROR("failed set ssl fd");
		muggle_socket_peer_close(peer);
		return;
	}

	// try ssl accept
	peer_data->ssl_established = 0;
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
}

void on_message(struct muggle_socket_event *ev, struct muggle_socket_peer *peer)
{
	user_peer_data_t *peer_data = peer->data;
	if (peer_data == NULL)
	{
		return;
	}
	WOLFSSL *ssl = peer_data->ssl;

	// ensure ssl accept done
	if (!peer_data->ssl_established)
	{
		LOG_INFO("on message, try ssl accept");

		// try ssl accept
		int ret = ssl_accept(ssl);
		if (ret == 0)
		{
			LOG_INFO("ssl accept done");
			peer_data->ssl_established = 1;
		}
		else if (ret == -1)
		{
			LOG_ERROR("failed ssl accept");
			muggle_socket_peer_close(peer);
			return;
		}
		else
		{
			// wait ssl accept done
			LOG_INFO("wait for ssl accept done");
			return;
		}
	}

	// echo message
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

		size_t len = strlen(buf);
		if (wolfSSL_write(ssl, buf, len) != len)
		{
			LOG_ERROR("failed wolfssl write");
			muggle_socket_peer_close(peer);
			break;
		}
	} while(1);
}

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

	// create listen socket
	const char *host = argv[1];
	const char *serv = argv[2];

	muggle_socket_peer_t peer;
	if (muggle_tcp_listen(host, serv, 512, &peer) == MUGGLE_INVALID_SOCKET)
	{
		MUGGLE_LOG_SYS_ERR(LOG_LEVEL_ERROR, "failed listen");
		LOG_ERROR("failed create tcp listen for %s:%s", host, serv);
		exit(EXIT_FAILURE);
	}

	// init ssl
	wolfSSL_Init();

	const char *cert_file = "certs/server.crt";
	const char *key_file = "certs/server.key";
	WOLFSSL_CTX *ctx = gen_ssl_ctx(cert_file, key_file);
	if (ctx == NULL)
	{
		LOG_ERROR("failed generate ssl ctx");
		exit(EXIT_FAILURE);
	}

	user_ev_data_t data;
	memset(&data, 0, sizeof(data));
	data.ctx = ctx;

	// fillup event
	muggle_socket_event_init_arg_t ev_init_arg;
	memset(&ev_init_arg, 0, sizeof(ev_init_arg));
	ev_init_arg.cnt_peer = 1;
	ev_init_arg.peers = &peer;
	ev_init_arg.on_connect = on_connect;
	ev_init_arg.on_error = on_error;
	ev_init_arg.on_close = on_close;
	ev_init_arg.on_message = on_message;
	ev_init_arg.datas = &data;

	// start event loop
	muggle_socket_event_t ev;
	if (muggle_socket_event_init(&ev_init_arg, &ev) != 0)
	{
		LOG_ERROR("failed init socket event");
		exit(EXIT_FAILURE);
	}
	muggle_socket_event_loop(&ev);

	// cleanup ssl
	wolfSSL_CTX_free(ctx);
	wolfSSL_Cleanup();
	
	return 0;
}
