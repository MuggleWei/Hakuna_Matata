#ifndef EXAMPLE_TCP_N_DATA_CLI_H_
#define EXAMPLE_TCP_N_DATA_CLI_H_

#include "cfg.h"

EXTERN_C_BEGIN

void run_client(muggle_event_loop_t *evloop,
				muggle_socket_evloop_handle_t *handle);

void client_on_add_ctx(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void client_on_message(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void client_on_close(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);
void client_on_release(muggle_event_loop_t *evloop, muggle_socket_context_t *ctx);

EXTERN_C_END

#endif // !EXAMPLE_TCP_N_DATA_CLI_H_
