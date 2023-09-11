#ifndef SSL_SERVER_H_
#define SSL_SERVER_H_

#include "muggle/c/muggle_c.h"

EXTERN_C_BEGIN

bool run_ssl_server(const char *host, const char *port);

// ================================
// event loop callbacks
// ================================

void ssl_serv_on_connect(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);

void ssl_serv_on_message(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);

void ssl_serv_on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);

void ssl_serv_on_release(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);

void ssl_serv_on_timer(muggle_event_loop_t *evloop);

EXTERN_C_END

#endif // !SSL_SERVER_H_
