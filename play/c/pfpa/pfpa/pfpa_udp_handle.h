#ifndef PFPA_UDP_HANDLE_H_
#define PFPA_UDP_HANDLE_H_

#include "pfpa/pfpa_macro.h"
#include "pfpa/pfpa_struct.h"
#include "pcap.h"

#include <netinet/udp.h>

EXTERN_C_BEGIN

PFPA_EXPORT
void pfpa_udp_packet_handle(pfpa_context_t *ctx,
							pfpa_packet_context_t *packet_ctx,
							struct udphdr *udp_hdr);

EXTERN_C_END

#endif // !PFPA_UDP_HANDLE_H_
