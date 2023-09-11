#ifndef SSL_SESSION_H_
#define SSL_SESSION_H_

#include "openssl/ssl.h"
#include "muggle/c/muggle_c.h"

typedef struct ssl_session {
	SSL *ssl;
	int status; //!< ssl connection status; 0 - not completed, 1 - completed
	char remote_addr[MUGGLE_SOCKET_ADDR_STRLEN];
} ssl_session_t;

ssl_session_t *ssl_session_new(muggle_socket_context_t *ctx, SSL_CTX *ssl_ctx);

void ssl_session_delete(ssl_session_t *session);

#define OUTPUT_SSL_SESSION_ERROR(session, err_code)          \
	{                                                        \
		LOG_ERROR("ssl error: "                              \
				  "remote_addr=%s, err_code=%d, err_msg=%s", \
				  session->remote_addr, err_code,            \
				  ERR_reason_error_string(ERR_get_error())); \
	}

#endif // !SSL_SESSION_H_
