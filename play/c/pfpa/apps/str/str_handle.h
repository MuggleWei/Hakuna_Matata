#ifndef STR_HANDLE_H_
#define STR_HANDLE_H_

#include "pfpa/pfpa.h"

EXTERN_C_BEGIN

void on_str_tcp_packet(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx,
					   pfpa_tcp_session_t *session);

void on_str_udp_packet(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx, void *data,
					   uint32_t datalen);

EXTERN_C_END

#endif // !STR_HANDLE_H_
