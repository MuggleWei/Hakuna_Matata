#include "pfpa_udp_handle.h"
#include "pfpa/pfpa_context.h"

static void pfpa_udp_output(pfpa_context_t *ctx,
							pfpa_packet_context_t *packet_ctx,
							struct udphdr *udp_hdr)
{
	MUGGLE_UNUSED(udp_hdr);
	fprintf(ctx->fp,
			"  - UDP|"
			"sport=%u;dport=%u;uhl=%u;datalen=%u\n",
			packet_ctx->src_port, packet_ctx->dst_port,
			(uint32_t)sizeof(struct udphdr), packet_ctx->datalen);
	fflush(ctx->fp);
}

void pfpa_udp_packet_handle(pfpa_context_t *ctx,
							pfpa_packet_context_t *packet_ctx,
							struct udphdr *udp_hdr)
{
	MUGGLE_UNUSED(udp_hdr);

	pfpa_udp_output(ctx, packet_ctx, udp_hdr);

	if (ctx->udp_callback) {
		ctx->udp_callback(ctx, packet_ctx, packet_ctx->data,
						  packet_ctx->datalen);
	}
}
