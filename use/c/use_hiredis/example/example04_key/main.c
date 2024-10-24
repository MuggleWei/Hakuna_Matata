#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hiredis.h"

bool auth(redisContext *ctx)
{
	redisReply *reply = NULL;
	bool ret = true;

	reply = redisCommand(ctx, "AUTH %s", "wsz123");
	if (reply == NULL) {
		fprintf(stderr, "auth error: can't allocate redis reply\n");
		ret = false;
		goto exit_auth;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		fprintf(stderr, "auth error: %s\n", reply->str);
		ret = false;
		goto exit_auth;
	}

	fprintf(stdout, "auth: %s\n", reply->str);

exit_auth:
	if (reply) {
		freeReplyObject(reply);
	}
	return ret;
}

bool ping(redisContext *ctx)
{
	redisReply *reply = NULL;
	bool ret = true;

	reply = redisCommand(ctx, "PING");
	if (reply == NULL) {
		fprintf(stderr, "ping error: can't allocate redis reply\n");
		ret = false;
		goto exit_auth;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		fprintf(stderr, "ping error: %s\n", reply->str);
		ret = false;
		goto exit_auth;
	}

	fprintf(stdout, "ping: %s\n", reply->str);

exit_auth:
	if (reply) {
		freeReplyObject(reply);
	}
	return ret;
}

bool set_key(redisContext *ctx, const char *k, const char *v)
{
	redisReply *reply = NULL;
	bool ret = true;

	reply = redisCommand(ctx, "SET %s %s", k, v);
	if (reply == NULL) {
		fprintf(stderr, "set error: can't allocate redis reply\n");
		ret = false;
		goto exit_auth;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		fprintf(stderr, "set error: %s\n", reply->str);
		ret = false;
		goto exit_auth;
	}

	fprintf(stdout, "set %s: %s\n", k, reply->str);

exit_auth:
	if (reply) {
		freeReplyObject(reply);
	}
	return ret;
}

bool get_key(redisContext *ctx, const char *k)
{
	redisReply *reply = NULL;
	bool ret = true;

	reply = redisCommand(ctx, "GET %s", k);
	if (reply == NULL) {
		fprintf(stderr, "get error: can't allocate redis reply\n");
		ret = false;
		goto exit_auth;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		fprintf(stderr, "get error: %s\n", reply->str);
		ret = false;
		goto exit_auth;
	}

	if (reply->str) {
		fprintf(stdout, "get %s: %s\n", k, reply->str);
	} else {
		fprintf(stdout, "get %s: (NULL)\n", k);
		ret = false;
	}

exit_auth:
	if (reply) {
		freeReplyObject(reply);
	}
	return ret;
}

int main()
{
	// connect redis
	struct timeval timeout = { 3, 0 };
	redisContext *ctx = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
	if (ctx == NULL || ctx->err) {
		if (ctx) {
			fprintf(stderr, "connection error: %s\n", ctx->errstr);
			redisFree(ctx);
		} else {
			fprintf(stderr, "connection error: can't allocate redis context\n");
		}
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "connected to redis\n");

	// auth
	auth(ctx);

	// ping
	ping(ctx);

	// set key
	const char *k = "foo";
	const char *v = "1";
	set_key(ctx, k, v);

	// get key
	get_key(ctx, k);
	get_key(ctx, "bar");

	// free redis context
	redisFree(ctx);

	return 0;
}
