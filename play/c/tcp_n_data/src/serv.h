#ifndef EXAMPLE_TCP_N_DATA_SERV_H_
#define EXAMPLE_TCP_N_DATA_SERV_H_

#include "cfg.h"

EXTERN_C_BEGIN

void run_server(muggle_event_loop_t *evloop,
				muggle_socket_evloop_handle_t *handle);

void serv_on_add_ctx(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void serv_on_connect(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void serv_on_message(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void serv_on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void serv_on_release(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);

EXTERN_C_END

#endif // !EXAMPLE_TCP_N_DATA_SERV_H_
