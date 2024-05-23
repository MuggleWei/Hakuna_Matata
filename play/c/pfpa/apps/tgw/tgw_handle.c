#include "tgw_handle.h"
#include "muggle/c/log/log.h"
#include "muggle/c/os/endian.h"
#include <stdlib.h>
#include <string.h>

; // for avoid LSP complain
#pragma pack(push)
#pragma pack(1)

typedef struct {
	uint32_t MsgType; //!< 消息类型
	uint32_t BodyLength; //!< 消息体长度
} tgw_hdr_t;

typedef struct {
	uint32_t checksum; //!< 校验和
} tgw_tail_t;

#pragma pack(pop)

void handle_tgw_immediate(pfpa_context_t *ctx,
						  pfpa_packet_context_t *packet_ctx,
						  pfpa_tcp_session_t *session)
{
	MUGGLE_UNUSED(session);

	if (packet_ctx->datalen < sizeof(tgw_hdr_t)) {
		LOG_WARNING("[%u] datalen < sizeof(tgw_hdr_t)", ctx->total_pack);
		return;
	}

	tgw_hdr_t *hdr = (tgw_hdr_t *)packet_ctx->data;
	hdr->MsgType = MUGGLE_ENDIAN_SWAP_32(hdr->MsgType);
	hdr->BodyLength = MUGGLE_ENDIAN_SWAP_32(hdr->BodyLength);

	if (packet_ctx->datalen ==
		sizeof(tgw_hdr_t) + hdr->BodyLength + sizeof(tgw_tail_t)) {
		fprintf(ctx->fp, "  - TGW|MsgType=%u;BodyLength=%u\n", hdr->MsgType,
				hdr->BodyLength);
		fflush(ctx->fp);
	} else {
		LOG_WARNING("[%u] datalen is incorrect", ctx->total_pack);
	}
}

void handle_tgw(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx,
				pfpa_tcp_session_t *session)
{
	muggle_bytes_buffer_t *bytes_buf = &session->bytes_buf;

	if (muggle_bytes_buffer_writable(bytes_buf) >= (int)packet_ctx->datalen) {
		muggle_bytes_buffer_write(bytes_buf, (int)packet_ctx->datalen,
								  packet_ctx->data);
	}

	while (1) {
		if (muggle_bytes_buffer_readable(bytes_buf) < (int)sizeof(tgw_hdr_t)) {
			break;
		}

		tgw_hdr_t tmphdr;
		muggle_bytes_buffer_fetch(bytes_buf, sizeof(tmphdr), &tmphdr);
		tmphdr.MsgType = MUGGLE_ENDIAN_SWAP_32(tmphdr.MsgType);
		tmphdr.BodyLength = MUGGLE_ENDIAN_SWAP_32(tmphdr.BodyLength);

		int total_len =
			sizeof(tgw_hdr_t) + tmphdr.BodyLength + sizeof(tgw_tail_t);
		if (muggle_bytes_buffer_readable(bytes_buf) < total_len) {
			LOG_WARNING("ignore total len: %d", total_len);
			break;
		}

		char *buf = NULL;
		tgw_hdr_t *hdr = muggle_bytes_buffer_reader_fc(bytes_buf, total_len);
		if (hdr == NULL) {
			buf = (char *)malloc(total_len);
			muggle_bytes_buffer_read(bytes_buf, total_len, buf);
			hdr = (tgw_hdr_t *)buf;
		}

		hdr->MsgType = MUGGLE_ENDIAN_SWAP_32(hdr->MsgType);
		hdr->BodyLength = MUGGLE_ENDIAN_SWAP_32(hdr->BodyLength);

		fprintf(ctx->fp, "  - TGW|MsgType=%u;BodyLength=%u\n", hdr->MsgType,
				hdr->BodyLength);
		fflush(ctx->fp);

		if (buf) {
			free(buf);
		} else {
			muggle_bytes_buffer_reader_move(bytes_buf, total_len);
		}
	}
}

void on_tcp_packet(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx,
				   pfpa_tcp_session_t *session)
{
	MUGGLE_UNUSED(packet_ctx);

	if (packet_ctx->datalen == 0) {
		return;
	}

	pfpa_addr_t *match_addr = (pfpa_addr_t *)ctx->user_data;

	if (memcmp(&session->key.src, match_addr, sizeof(pfpa_addr_t)) != 0 &&
		memcmp(&session->key.dst, match_addr, sizeof(pfpa_addr_t)) != 0) {
		return;
	}

	if (1) {
		// NOTE:
		// assume network environment is good and half-packets/sticky-packets
		// will not occur
		handle_tgw_immediate(ctx, packet_ctx, session);
	} else {
		handle_tgw(ctx, packet_ctx, session);
	}
}
