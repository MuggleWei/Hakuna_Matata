#ifndef PFPA_STRUCT_H_
#define PFPA_STRUCT_H_

#include "muggle/c/dsaa/hash_table.h"
#include "muggle/c/memory/bytes_buffer.h"
#include "muggle/c/net/socket.h"
#include "pfpa/pfpa_macro.h"
#include <stdio.h>
#include <time.h>

EXTERN_C_BEGIN

/**
 * @brief filter tuple
 */
typedef struct {
	uint32_t addr;
	uint16_t port;
} pfpa_addr_t;

/**
 * @brief PFPA connection key
 */
typedef struct {
	pfpa_addr_t src;
	pfpa_addr_t dst;
} pfpa_conn_key_t;

enum {
	PFPA_TCP_STATUS_NULL = 0,
	PFPA_TCP_STATUS_SYN_SENT,
	PFPA_TCP_STATUS_SYN_ACK,
	PFPA_TCP_STATUS_ESTABLISHED,
	PFPA_TCP_STATUS_FIN_SENT,
	PFPA_TCP_STATUS_FIN_ACK,
};

/**
 * @brief pfpa tcp session
 */
typedef struct {
	pfpa_conn_key_t key;
	uint8_t status;
	uint32_t seq;
	muggle_bytes_buffer_t bytes_buf;
} pfpa_tcp_session_t;

/**
 * @brief packet context
 */
typedef struct {
	struct tm t;
	uint32_t ns;
	uint32_t src_addr;
	uint32_t dst_addr;
	uint16_t src_port; //!< src port, already convert endianess
	uint16_t dst_port; //!< dst port, already convert endianess
	char src_ip[MUGGLE_SOCKET_ADDR_STRLEN];
	char dst_ip[MUGGLE_SOCKET_ADDR_STRLEN];
	void *data;
	uint32_t datalen;
} pfpa_packet_context_t;

struct pfpa_context;

typedef void (*pfpa_fn_tcp_callback)(struct pfpa_context *ctx,
									 pfpa_packet_context_t *packet_ctx,
									 pfpa_tcp_session_t *session);

typedef void (*pfpa_fn_udp_callback)(struct pfpa_context *ctx,
									 pfpa_packet_context_t *packet_ctx,
									 void *data, uint32_t datalen);

/**
 * @brief PFPA context
 */
typedef struct pfpa_context {
	FILE *fp; //!< output file
	uint32_t total_pack; //!< total number of packet

	bool enable_fileter; //!< enable filter

	// k: pfpa_addr_t*, v: pfpa_addr_t*
	muggle_hash_table_t filter_dict;

	// k: pfpa_conn_key_t*, v: pfpa_tcp_session_t*
	muggle_hash_table_t tcp_session_dict;

	pfpa_fn_tcp_callback tcp_callback;

	pfpa_fn_udp_callback udp_callback;

	void *user_data;
} pfpa_context_t;

EXTERN_C_END

#endif // !PFPA_STRUCT_H_
