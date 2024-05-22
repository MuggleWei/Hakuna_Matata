#include "pfpa_tcp_handle.h"
#include "muggle/c/log/log.h"
#include "pfpa/pfpa_context.h"
#include <stdlib.h>
#include <string.h>

static void pfpa_tcp_output(pfpa_context_t *ctx,
							pfpa_packet_context_t *packet_ctx,
							struct tcphdr *tcp_hdr)
{
	// output TCP
	char flags[32];
	pfpa_tcp_get_flags(tcp_hdr->th_flags, flags);
	fprintf(ctx->fp,
			"  - TCP|"
			"sport=%u;dport=%u;flags=[%s];thl=%u;datalen=%u;seq=%u;ack=%u\n",
			packet_ctx->src_port, packet_ctx->dst_port, flags,
			tcp_hdr->th_off * 4, packet_ctx->datalen, ntohl(tcp_hdr->seq),
			ntohl(tcp_hdr->ack_seq));
	fflush(ctx->fp);
}

static void pfpa_tcp_data_handle(pfpa_context_t *ctx,
								 pfpa_packet_context_t *packet_ctx,
								 struct tcphdr *tcp_hdr,
								 pfpa_tcp_session_t *session)
{
	MUGGLE_UNUSED(tcp_hdr);

	muggle_bytes_buffer_t *bytes_buf = &session->bytes_buf;
	if (muggle_bytes_buffer_writable(bytes_buf) < (int)packet_ctx->datalen) {
		return;
	}
	muggle_bytes_buffer_write(bytes_buf, (int)packet_ctx->datalen,
							  packet_ctx->data);

	if (ctx->tcp_callback) {
		ctx->tcp_callback(ctx, packet_ctx, session);
	}
}

void pfpa_tcp_packet_handle(pfpa_context_t *ctx,
							pfpa_packet_context_t *packet_ctx,
							struct tcphdr *tcp_hdr)
{
	pfpa_tcp_output(ctx, packet_ctx, tcp_hdr);

	// session
	pfpa_conn_key_t conn_key;
	memset(&conn_key, 0, sizeof(conn_key));
	conn_key.src.addr = packet_ctx->src_addr;
	conn_key.src.port = packet_ctx->src_port;
	conn_key.dst.addr = packet_ctx->dst_addr;
	conn_key.dst.port = packet_ctx->dst_port;
	pfpa_tcp_session_t *session = pfpa_ctx_find_tcp_session(ctx, &conn_key);
	if (session == NULL) {
		LOG_DEBUG("TCP connection: [%u] %s:%u->%s:%u", ctx->total_pack,
				  packet_ctx->src_ip, packet_ctx->src_port, packet_ctx->dst_ip,
				  packet_ctx->dst_port);
		session = (pfpa_tcp_session_t *)malloc(sizeof(pfpa_tcp_session_t));
		memset(session, 0, sizeof(pfpa_tcp_session_t));
		memcpy(&session->key, &conn_key, sizeof(pfpa_conn_key_t));
		muggle_bytes_buffer_init(&session->bytes_buf, 64 * 1024);

		pfpa_ctx_add_tcp_session(ctx, session);

		session->seq = ntohl(tcp_hdr->seq);
		session->status = PFPA_TCP_STATUS_ESTABLISHED;
	}

	uint32_t seq = ntohl(tcp_hdr->seq);
	if (session->seq != seq) {
		LOG_WARNING("unexpected seq: expected=%u, actual=%u, [%u] %s:%u->%s:%u",
					session->seq, seq, ctx->total_pack, packet_ctx->src_ip,
					packet_ctx->src_port, packet_ctx->dst_ip,
					packet_ctx->dst_port);
	}

	// handle tcp data
	pfpa_tcp_data_handle(ctx, packet_ctx, tcp_hdr, session);

	// handle seq
	if (tcp_hdr->syn || tcp_hdr->fin) {
		session->seq += 1;
	} else {
		session->seq = seq + packet_ctx->datalen;
	}

	// handle disconnect
	if (session->status == PFPA_TCP_STATUS_FIN_SENT && tcp_hdr->ack) {
		pfpa_conn_key_t remote_key;
		remote_key.src.addr = conn_key.dst.addr;
		remote_key.src.port = conn_key.dst.port;
		remote_key.dst.addr = conn_key.src.addr;
		remote_key.dst.port = conn_key.src.port;
		pfpa_tcp_session_t *remote_session =
			pfpa_ctx_find_tcp_session(ctx, &remote_key);
		if (remote_session &&
			remote_session->status == PFPA_TCP_STATUS_FIN_SENT) {
			LOG_DEBUG("TCP disconnect: [%u] %s:%u->%s:%u", ctx->total_pack,
					  packet_ctx->src_ip, packet_ctx->src_port,
					  packet_ctx->dst_ip, packet_ctx->dst_port);
			pfpa_ctx_remove_tcp_session(ctx, &conn_key);
			LOG_DEBUG("TCP disconnect: [%u] %s:%u->%s:%u", ctx->total_pack,
					  packet_ctx->dst_ip, packet_ctx->dst_port,
					  packet_ctx->src_ip, packet_ctx->src_port);
			pfpa_ctx_remove_tcp_session(ctx, &remote_key);
		}
	}

	// if fin, need check is fin sent or fin ack
	if (tcp_hdr->fin) {
		session->status = PFPA_TCP_STATUS_FIN_SENT;
	}
}

void pfpa_tcp_get_flags(uint8_t tcp_flags, char *buf)
{
	uint32_t pos = 0;

	uint32_t short_len = 3;
	uint32_t long_len = 4;
	if (tcp_flags & TH_FIN) {
		if (pos == 0) {
			memcpy(&buf[pos], "FIN", short_len);
			pos += short_len;
		} else {
			memcpy(&buf[pos], "|FIN", long_len);
			pos += long_len;
		}
	}
	if (tcp_flags & TH_SYN) {
		if (pos == 0) {
			memcpy(&buf[pos], "SYN", short_len);
			pos += short_len;
		} else {
			memcpy(&buf[pos], "|SYN", long_len);
			pos += long_len;
		}
	}
	if (tcp_flags & TH_RST) {
		if (pos == 0) {
			memcpy(&buf[pos], "RST", short_len);
			pos += short_len;
		} else {
			memcpy(&buf[pos], "|RST", long_len);
			pos += long_len;
		}
	}
	if (tcp_flags & TH_PUSH) {
		if (pos == 0) {
			memcpy(&buf[pos], "PSH", short_len);
			pos += short_len;
		} else {
			memcpy(&buf[pos], "|PSH", long_len);
			pos += long_len;
		}
	}
	if (tcp_flags & TH_ACK) {
		if (pos == 0) {
			memcpy(&buf[pos], "ACK", short_len);
			pos += short_len;
		} else {
			memcpy(&buf[pos], "|ACK", long_len);
			pos += long_len;
		}
	}
	if (tcp_flags & TH_URG) {
		if (pos == 0) {
			memcpy(&buf[pos], "URG", short_len);
			pos += short_len;
		} else {
			memcpy(&buf[pos], "|URG", long_len);
			pos += long_len;
		}
	}

	buf[pos] = '\0';
}
