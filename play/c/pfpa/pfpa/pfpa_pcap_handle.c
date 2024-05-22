#include "pfpa_pcap_handle.h"
#include "muggle/c/log/log.h"
#include "pfpa/pfpa_context.h"
#include "pfpa/pfpa_tcp_handle.h"
#include "pfpa/pfpa_udp_handle.h"
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <string.h>

static void pfpa_output_ip_infos(pfpa_context_t *ctx,
								 pfpa_packet_context_t *packet_ctx,
								 const struct pcap_pkthdr *pcap_hdr,
								 const struct ether_header *eth_hdr,
								 const struct iphdr *ip_hdr,
								 uint32_t eth_padding, uint32_t ip_hdr_len,
								 uint32_t ip_len)
{
	// packet layer
	fprintf(ctx->fp,
			"[%u]|ts=%02d:%02d:%02d.%09u;%s:%u->%s:%u;caplen=%u;len=%u\n",
			ctx->total_pack, packet_ctx->t.tm_hour, packet_ctx->t.tm_min,
			packet_ctx->t.tm_sec, packet_ctx->ns, packet_ctx->src_ip,
			packet_ctx->src_port, packet_ctx->dst_ip, packet_ctx->dst_port,
			pcap_hdr->caplen, pcap_hdr->len);

	// ether protocol
	fprintf(ctx->fp,
			"  - ether|"
			"dst=%02x:%02x:%02x:%02x:%02x:%02x;"
			"src=%02x:%02x:%02x:%02x:%02x:%02x;"
			"ehl=%u;padding=%u\n",
			eth_hdr->ether_dhost[0], eth_hdr->ether_dhost[1],
			eth_hdr->ether_dhost[2], eth_hdr->ether_dhost[3],
			eth_hdr->ether_dhost[4], eth_hdr->ether_dhost[5],
			eth_hdr->ether_shost[0], eth_hdr->ether_shost[1],
			eth_hdr->ether_shost[2], eth_hdr->ether_shost[3],
			eth_hdr->ether_shost[4], eth_hdr->ether_shost[5],
			(uint32_t)sizeof(struct ether_header), eth_padding);

	// ip protocol
	fprintf(ctx->fp,
			"  - IPv4|"
			"saddr=%s;daddr=%s;ihl=%u(%u);tot_len=%u;"
			"\n",
			packet_ctx->src_ip, packet_ctx->dst_ip, ip_hdr->ihl, ip_hdr_len,
			ip_len);
}

void pfpa_packet_handle(u_char *args, const struct pcap_pkthdr *header,
						const u_char *packet)
{
	pfpa_context_t *ctx = (pfpa_context_t *)args;

	++ctx->total_pack;
	if (ctx->total_pack % 1000 == 0) {
		LOG_INFO("handle packet num: %u", ctx->total_pack);
	}

	struct ether_header *eth_hdr = (struct ether_header *)packet;
	uint16_t ether_type = ntohs(eth_hdr->ether_type);

	switch (ether_type) {
	case ETHERTYPE_IP: {
		pfpa_ipv4_packet_handle(ctx, header, packet);
	} break;
	case ETHERTYPE_IPV6: {
		LOG_WARNING("ignore ipv6");
	} break;
	default: {
		LOG_WARNING("unknonw ether type: 0x%x", ether_type);
	} break;
	}
}

void pfpa_ipv4_packet_handle(pfpa_context_t *ctx,
							 const struct pcap_pkthdr *pcap_hdr,
							 const u_char *packet)
{
	const struct iphdr *ip_hdr =
		(const struct iphdr *)(packet + sizeof(struct ether_header));
	uint32_t ip_hdr_len = ip_hdr->ihl * 4;

	// NOTE:
	// if TCP disable "Round-Trip Time Measurement" option that defined in
	// RFC7323, then may have padding bytes in packet
	uint32_t ip_len = ntohs(ip_hdr->tot_len);
	uint32_t total_len = ip_len + sizeof(struct ether_header);
	uint32_t eth_padding = 0;
	if (total_len < pcap_hdr->caplen) {
		eth_padding = pcap_hdr->caplen - total_len;
	}

	// get packet context
	pfpa_packet_context_t packet_ctx;
	memset(&packet_ctx, 0, sizeof(packet_ctx));

	time_t sec = (time_t)pcap_hdr->ts.tv_sec;
	localtime_r(&sec, &packet_ctx.t);
	packet_ctx.ns = pcap_hdr->ts.tv_usec;

	packet_ctx.src_addr = ip_hdr->saddr;
	packet_ctx.dst_addr = ip_hdr->daddr;

	inet_ntop(AF_INET, &(ip_hdr->saddr), packet_ctx.src_ip, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(ip_hdr->daddr), packet_ctx.dst_ip, INET_ADDRSTRLEN);

	// parse protocol
	switch (ip_hdr->protocol) {
	case IPPROTO_TCP: {
		struct tcphdr *tcp_hdr =
			(struct tcphdr *)(packet + sizeof(struct ether_header) +
							  ip_hdr_len);

		packet_ctx.src_port = ntohs(tcp_hdr->th_sport);
		packet_ctx.dst_port = ntohs(tcp_hdr->th_dport);

		uint32_t tcp_hdr_len = tcp_hdr->th_off * 4;
		packet_ctx.data = (u_char *)tcp_hdr + tcp_hdr_len;
		packet_ctx.datalen = pcap_hdr->caplen - eth_padding -
							 sizeof(struct ether_header) - ip_hdr_len -
							 tcp_hdr_len;

		if (!pfpa_ctx_check_filter(ctx, &packet_ctx)) {
			return;
		}

		pfpa_output_ip_infos(ctx, &packet_ctx, pcap_hdr,
							 (struct ether_header *)packet, ip_hdr, eth_padding,
							 ip_hdr_len, ip_len);

		pfpa_tcp_packet_handle(ctx, &packet_ctx, tcp_hdr);
	} break;
	case IPPROTO_UDP: {
		struct udphdr *udp_hdr =
			(struct udphdr *)(packet + sizeof(struct ether_header) +
							  ip_hdr_len);
		packet_ctx.src_port = ntohs(udp_hdr->uh_sport);
		packet_ctx.dst_port = ntohs(udp_hdr->uh_dport);

		packet_ctx.data = (u_char *)udp_hdr + sizeof(struct udphdr);
		packet_ctx.datalen = ntohs(udp_hdr->len) - sizeof(struct udphdr);

		if (!pfpa_ctx_check_filter(ctx, &packet_ctx)) {
			return;
		}

		pfpa_output_ip_infos(ctx, &packet_ctx, pcap_hdr,
							 (struct ether_header *)packet, ip_hdr, eth_padding,
							 ip_hdr_len, ip_len);

		pfpa_udp_packet_handle(ctx, &packet_ctx, udp_hdr);
	} break;
	default: {
		LOG_WARNING("unknown protocol: %d", (int)ip_hdr->protocol);
	} break;
	}
}
