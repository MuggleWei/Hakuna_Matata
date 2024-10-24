#include <stdio.h>
#include <stdlib.h>
#include "hiredis.h"

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

	// free redis context
	redisFree(ctx);

	return 0;
}
