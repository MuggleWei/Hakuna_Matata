#ifndef PACKET_HANDLE_H_
#define PACKET_HANDLE_H_

#include "muggle/c/muggle_c.h"
#include "pcap.h"
#include <netinet/in.h>
#include <net/ethernet.h>

EXTERN_C_BEGIN

/**
 * @brief packet handle
 *
 * @param args    user costomized arguments
 * @param header  packet header
 * @param packet  packet
 */
void packet_handle(
	u_char *args,
	const struct pcap_pkthdr *header,
	const u_char *packet);

/**
 * @brief IPv4 packet callback handler
 *
 * @param header  packet header
 * @param packet  packet
 */
void ipv4_packet_handler(
	const struct pcap_pkthdr *header,
	const u_char *packet);

/**
 * @brief IPv6 packet callback handler
 *
 * @param header  packet header
 * @param packet  packet
 */
void ipv6_packet_handler(
	const struct pcap_pkthdr *header,
	const u_char *packet);

EXTERN_C_END

#endif /* ifndef PACKET_HANDLE_H_ */
