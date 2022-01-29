#include "packet_handle.h"

#include <netinet/ip.h>
#include <netinet/tcp.h>

void packet_handle(
	u_char *args,
	const struct pcap_pkthdr *header,
	const u_char *packet)
{
	struct ether_header *eth_header = (struct ether_header*)packet;
	uint16_t ether_type = ntohs(eth_header->ether_type);
	switch (ether_type)
	{
	case ETHERTYPE_IP:
		{
			ipv4_packet_handler(header, packet);
		}break;
	case ETHERTYPE_IPV6:
		{
			ipv6_packet_handler(header, packet);
		}break;
	default:
		{
			LOG_INFO("ignore packet");
		}break;
	}
}

void ipv4_packet_handler(
	const struct pcap_pkthdr *header,
	const u_char *packet)
{
	// caplen: the amount actually available
	// len: the total packet length even if it is larger than what we currently have captured
	// if snapshot length set with pcap_open_live() is too small, you may not have the whole packet
	LOG_INFO("-----------------------------------------");
	LOG_INFO("total packet available: %d bytes", header->caplen);
	LOG_INFO("expected packet size: %d bytes", header->len);
	if (header->caplen < header->len)
	{
		// TODO: ignore caplen < len
		LOG_TRACE("capture length < packet length, ignore");
		return;
	}

	LOG_INFO("============== Ether ==============");
	LOG_INFO("ether header length: %d", (int)sizeof(struct ether_header));

	// get ipv4 packet header
	const struct iphdr *ip_header = (const struct iphdr*)(packet + sizeof(struct ether_header));
	unsigned int ihl = ip_header->ihl * 4; // IHL: how many 4bytes
	const char *str_protocol = "unknown";
	char protocol_buf[32];
	switch (ip_header->protocol)
	{
	case IPPROTO_TCP:
		{
			str_protocol = "TCP";
		}break;
	case IPPROTO_UDP:
		{
			str_protocol = "UDP";
		}break;
	default:
		{
			snprintf(protocol_buf, sizeof(protocol_buf), "IPPROTO number: %d", (int)ip_header->protocol);
		}break;
	}

	LOG_INFO("============== IP ==============");
	LOG_INFO("version: %u", ip_header->version);
	LOG_INFO("IHL(IP header length) in bytes: %u", ihl); 
	LOG_INFO("protocol: %s", str_protocol);

	switch (ip_header->protocol)
	{
	case IPPROTO_TCP:
		{
			struct tcphdr *tcp_header = (struct tcphdr*)(packet + sizeof(struct ether_header) + ihl);
			unsigned int tcp_header_len = tcp_header->doff * 4;

			LOG_INFO("============== TCP ==============");
			LOG_INFO("TCP header length in bytes: %u", tcp_header_len);

			LOG_INFO("============== payload ==============");
			int total_header_size = (int)(sizeof(struct ether_header) + ihl + tcp_header_len);
			LOG_INFO("payload size: %d", (int)(header->len - total_header_size));
			LOG_INFO("payload memory address: 0x%llx", packet + total_header_size);
		}break;
	case IPPROTO_UDP:
		{
			// TODO:
		}break;
	}
}

void ipv6_packet_handler(
	const struct pcap_pkthdr *header,
	const u_char *packet)
{
	// TODO:
}
