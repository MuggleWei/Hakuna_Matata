#ifndef PFPA_TCP_HANDLE_H_
#define PFPA_TCP_HANDLE_H_

#include "pfpa/pfpa_macro.h"
#include "pfpa/pfpa_struct.h"
#include "pcap.h"

EXTERN_C_BEGIN

PFPA_EXPORT
void pfpa_tcp_packet_handle(pfpa_context_t *ctx,
							pfpa_packet_context_t *packet_ctx,
							struct tcphdr *tcp_hdr);

PFPA_EXPORT
void pfpa_tcp_get_flags(uint8_t tcp_flags, char *buf);

EXTERN_C_END

#endif // !PFPA_TCP_HANDLE_H_
