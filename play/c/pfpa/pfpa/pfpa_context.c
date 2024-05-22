#include "pfpa_context.h"
#include "muggle/c/log/log.h"
#include "pfpa/pfpa_murmurhash3.h"
#include <stdlib.h>
#include <string.h>

static uint64_t pfpa_filter_hash(void *p)
{
	uint32_t hash_key;
	pfpa_MurmurHash3_x86_32(p, sizeof(pfpa_addr_t), 0xB0F57EE3, &hash_key);
	return hash_key;
}
static int pfpa_filter_cmp(const void *d1, const void *d2)
{
	return memcmp(d1, d2, sizeof(pfpa_addr_t));
}
static void pfpa_hashtable_free_filter_addr(void *pool, void *data)
{
	MUGGLE_UNUSED(pool);
	free(data);
}

static uint64_t pfpa_tcp_session_hash(void *p)
{
	uint32_t hash_key;
	pfpa_MurmurHash3_x86_32(p, sizeof(pfpa_conn_key_t), 0xB0F57EE3, &hash_key);
	return hash_key;
}
static int pfpa_tcp_session_cmp(const void *d1, const void *d2)
{
	return memcmp(d1, d2, sizeof(pfpa_conn_key_t));
}
static void pfpa_hashtable_free_tcp_session(void *pool, void *data)
{
	MUGGLE_UNUSED(pool);
	pfpa_tcp_session_t *session = (pfpa_tcp_session_t *)data;
	muggle_bytes_buffer_destroy(&session->bytes_buf);
	free(session);
}

bool pfpa_ctx_init(pfpa_context_t *ctx)
{
	memset(ctx, 0, sizeof(*ctx));

	if (!muggle_hash_table_init(&ctx->filter_dict, HASH_TABLE_SIZE_10007,
								pfpa_filter_hash, pfpa_filter_cmp, 0)) {
		LOG_ERROR("failed init filter hash table");
		return false;
	}

	if (!muggle_hash_table_init(&ctx->tcp_session_dict, HASH_TABLE_SIZE_10007,
								pfpa_tcp_session_hash, pfpa_tcp_session_cmp,
								0)) {
		LOG_ERROR("failed init tcp session hash table");
		return false;
	}

	return true;
}

void pfpa_ctx_destroy(pfpa_context_t *ctx)
{
	muggle_hash_table_destroy(&ctx->tcp_session_dict, NULL, NULL,
							  pfpa_hashtable_free_tcp_session, NULL);
	muggle_hash_table_destroy(&ctx->filter_dict, NULL, NULL,
							  pfpa_hashtable_free_filter_addr, NULL);
}

void pfpa_ctx_add_filter(pfpa_context_t *ctx, pfpa_addr_t *addr)
{
	char buf[MUGGLE_SOCKET_ADDR_STRLEN];
	inet_ntop(AF_INET, &(addr->addr), buf, INET_ADDRSTRLEN);
	LOG_DEBUG("filter addr: [%s:%d]", buf, addr->port);

	muggle_hash_table_put(&ctx->filter_dict, addr, addr);
}

bool pfpa_ctx_check_filter(pfpa_context_t *ctx,
						   pfpa_packet_context_t *packet_ctx)
{
	if (!ctx->enable_fileter) {
		return true;
	}

	pfpa_addr_t tuple;

	tuple.addr = packet_ctx->src_addr;
	tuple.port = 0;
	if (muggle_hash_table_find(&ctx->filter_dict, &tuple)) {
		return true;
	}
	tuple.port = packet_ctx->src_port;
	if (muggle_hash_table_find(&ctx->filter_dict, &tuple)) {
		return true;
	}

	tuple.addr = packet_ctx->dst_addr;
	tuple.port = 0;
	if (muggle_hash_table_find(&ctx->filter_dict, &tuple)) {
		return true;
	}
	tuple.port = packet_ctx->dst_port;
	if (muggle_hash_table_find(&ctx->filter_dict, &tuple)) {
		return true;
	}

	return false;
}

void pfpa_ctx_add_tcp_session(pfpa_context_t *ctx, pfpa_tcp_session_t *session)
{
	muggle_hash_table_put(&ctx->tcp_session_dict, &session->key, session);
}

void pfpa_ctx_remove_tcp_session(pfpa_context_t *ctx, pfpa_conn_key_t *key)
{
	muggle_hash_table_node_t *node =
		muggle_hash_table_find(&ctx->tcp_session_dict, key);
	if (node) {
		muggle_hash_table_remove(&ctx->tcp_session_dict, node, NULL, NULL,
								 pfpa_hashtable_free_tcp_session, NULL);
	}
}

pfpa_tcp_session_t *pfpa_ctx_find_tcp_session(pfpa_context_t *ctx,
											  pfpa_conn_key_t *k)
{
	muggle_hash_table_node_t *node =
		(muggle_hash_table_node_t *)muggle_hash_table_find(
			&ctx->tcp_session_dict, k);
	if (node == NULL) {
		return NULL;
	}

	return (pfpa_tcp_session_t *)node->value;
}
