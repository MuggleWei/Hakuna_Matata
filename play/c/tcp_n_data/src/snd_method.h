#ifndef EXAMPLE_TCP_N_DATA_SND_METHOD_H_
#define EXAMPLE_TCP_N_DATA_SND_METHOD_H_

#include "cfg.h"
#include "data.h"

EXTERN_C_BEGIN

muggle_thread_ret_t snd_data_thread_routine(void *p_args);

void method_write(muggle_socket_context_t *ctx, data_t **p_datas, size_t n);
void method_writev(muggle_socket_context_t *ctx, data_t **p_datas, size_t n);
void method_big_write(muggle_socket_context_t *ctx, data_t **p_datas, size_t n);
void method_assemble_write(muggle_socket_context_t *ctx, data_t **p_datas,
						   size_t n);
void method_assemble_writev(muggle_socket_context_t *ctx, data_t **p_datas,
							size_t n);

EXTERN_C_END

#endif // !EXAMPLE_TCP_N_DATA_SND_METHOD_H_
