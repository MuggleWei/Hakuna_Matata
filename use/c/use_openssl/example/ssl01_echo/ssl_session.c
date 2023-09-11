#include "ssl_session.h"
#include "muggle/c/muggle_c.h"
#include "openssl/err.h"

ssl_session_t *ssl_session_new(muggle_socket_context_t *ctx, SSL_CTX *ssl_ctx)
{
	ssl_session_t *session = (ssl_session_t *)malloc(sizeof(ssl_session_t));
	if (session == NULL) {
		LOG_ERROR("failed allocate memory space for ssl session");
		return session;
	}

	memset(session, 0, sizeof(*session));

	session->ssl = SSL_new(ssl_ctx);
	if (session->ssl == NULL) {
		LOG_ERROR("failed new ssl");
		free(session);
		return NULL;
	}
	SSL_set_fd(session->ssl, muggle_socket_ctx_get_fd(ctx));

	session->status = 0;
	muggle_socket_remote_addr(muggle_socket_ctx_get_fd(ctx),
							  session->remote_addr,
							  sizeof(session->remote_addr), 0);

	return session;
}

void ssl_session_delete(ssl_session_t *session)
{
	if (session) {
		if (session->ssl) {
			SSL_shutdown(session->ssl);
			SSL_free(session->ssl);
			session->ssl = NULL;
		}
		free(session);
	}
}
