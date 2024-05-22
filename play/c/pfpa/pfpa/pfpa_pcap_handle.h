#ifndef PFPA_PCAP_HANDLE_H_
#define PFPA_PCAP_HANDLE_H_

#include "pfpa/pfpa_macro.h"
#include "pfpa/pfpa_struct.h"
#include "pcap.h"

EXTERN_C_BEGIN

PFPA_EXPORT
void pfpa_packet_handle(u_char *args, const struct pcap_pkthdr *header,
						const u_char *packet);

PFPA_EXPORT
void pfpa_ipv4_packet_handle(pfpa_context_t *ctx,
							 const struct pcap_pkthdr *pcap_hdr,
							 const u_char *packet);

EXTERN_C_END

#endif // !PFPA_PCAP_HANDLE_H_
