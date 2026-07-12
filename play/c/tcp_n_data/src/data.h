#ifndef TCP_N_DATA_DATA_H_
#define TCP_N_DATA_DATA_H_

#include "cfg.h"

EXTERN_C_BEGIN

typedef struct {
	muggle_event_loop_t *evloop;
	cfg_t *cfg;
} evloop_data_t;

typedef struct {
	char local_ip[MUGGLE_SOCKET_ADDR_STRLEN];
	int local_port;
	char remote_ip[MUGGLE_SOCKET_ADDR_STRLEN];
	int remote_port;
	int rcv_bufsize;
	int snd_bufsize;

	// client only
	cfg_t *cfg;
	void *datas;
	size_t total_data_nbytes;
	size_t total_rcv_nbytes;
	muggle_time_counter_t w_tc;
	muggle_time_counter_t r_tc;
	muggle_thread_t thread_snd;
} session_data_t;

typedef struct {
	char buf[320];
} data_t;

typedef struct {
	muggle_socket_iovec_t *iov;
	int iovcnt;
	int nbytes;
} batch_iov_t;

void gen_session_info(muggle_socket_context_t *ctx);
session_data_t *get_session_info(muggle_socket_context_t *ctx);
void release_session_info(muggle_socket_context_t *ctx);

void set_socket_opt_bufsize(muggle_socket_t fd, void *p);

EXTERN_C_END

#endif // !TCP_N_DATA_DATA_H_
