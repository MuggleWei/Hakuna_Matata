#ifndef PFPA_CONTEXT_H_
#define PFPA_CONTEXT_H_

#include "pfpa/pfpa_macro.h"
#include "pfpa/pfpa_struct.h"

EXTERN_C_BEGIN

PFPA_EXPORT
bool pfpa_ctx_init(pfpa_context_t *ctx);

PFPA_EXPORT
void pfpa_ctx_destroy(pfpa_context_t *ctx);

PFPA_EXPORT
void pfpa_ctx_add_filter(pfpa_context_t *ctx, pfpa_addr_t *addr);

PFPA_EXPORT
bool pfpa_ctx_check_filter(pfpa_context_t *ctx,
						   pfpa_packet_context_t *packet_ctx);

PFPA_EXPORT
void pfpa_ctx_add_tcp_session(pfpa_context_t *ctx, pfpa_tcp_session_t *session);

PFPA_EXPORT
void pfpa_ctx_remove_tcp_session(pfpa_context_t *ctx, pfpa_conn_key_t *key);

PFPA_EXPORT
pfpa_tcp_session_t *pfpa_ctx_find_tcp_session(pfpa_context_t *ctx,
											  pfpa_conn_key_t *k);

EXTERN_C_END

#endif // !PFPA_CONTEXT_H_
