#ifndef TGW_HANDLE_H_
#define TGW_HANDLE_H_

#include "pfpa/pfpa.h"

EXTERN_C_BEGIN

void on_tcp_packet(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx,
				   pfpa_tcp_session_t *session);

EXTERN_C_END

#endif // !TGW_HANDLE_H_
