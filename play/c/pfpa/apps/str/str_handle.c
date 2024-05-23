#include "str_handle.h"
#include "muggle/c/log/log.h"

void on_str_tcp_packet(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx,
					   pfpa_tcp_session_t *session)
{
	MUGGLE_UNUSED(packet_ctx);

	if (packet_ctx->datalen == 0) {
		return;
	}

	muggle_bytes_buffer_t *bytes_buf = &session->bytes_buf;

	if (muggle_bytes_buffer_writable(bytes_buf) >= (int)packet_ctx->datalen) {
		muggle_bytes_buffer_write(bytes_buf, (int)packet_ctx->datalen,
								  packet_ctx->data);
	}

	while (1) {
		int readable = muggle_bytes_buffer_contiguous_readable(bytes_buf);
		if (readable <= 0) {
			break;
		}

		const char *str = muggle_bytes_buffer_reader_fc(bytes_buf, readable);
		if (str == NULL) {
			LOG_FATAL("failed read");
			break;
		}

		fprintf(ctx->fp, "  - Application|s=%.*s\n", readable, str);
		fflush(ctx->fp);

		muggle_bytes_buffer_reader_move(bytes_buf, readable);
	}
}

void on_str_udp_packet(pfpa_context_t *ctx, pfpa_packet_context_t *packet_ctx,
					   void *data, uint32_t datalen)
{
	MUGGLE_UNUSED(packet_ctx);

	pfpa_context_t *p_ctx = (pfpa_context_t *)ctx;
	fprintf(p_ctx->fp, "  - Application|s=%.*s\n", datalen, (char *)data);
	fflush(p_ctx->fp);
}
