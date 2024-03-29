#include "muggle/c/muggle_c.h"

typedef struct {
	int a;
	int b;
} foo_t;

typedef struct {
	int a;
	char cacheline[64];
	int b;
} bar_t;

static uint32_t iter_cnt = 10000;
static int ready = 0;

muggle_thread_ret_t increase(void *args)
{
	int *p = (int*)args;

	while (muggle_atomic_load(&ready, muggle_memory_order_relaxed) == 0) {
	}

	struct timespec ts1, ts2;
	muggle_realtime_get(ts1);

	for (uint32_t i = 0; i < iter_cnt; ++i) {
		muggle_atomic_fetch_add(p, 1, muggle_memory_order_relaxed);
	}

	muggle_realtime_get(ts2);

	unsigned long long elapsed =
		(ts2.tv_sec - ts1.tv_sec) * 1000000000 + ts2.tv_nsec - ts1.tv_nsec;
	LOG_INFO("increase use time: %.3f us", elapsed / 1000.0);

	return NULL;
}

int main(int argc, char *argv[])
{
	iter_cnt *= argc;
	MUGGLE_UNUSED(argv);

	muggle_log_simple_init(LOG_LEVEL_DEBUG, -1);

	// false sharing
	{
		ready = 0;

		foo_t data;
		memset(&data, 0, sizeof(data));

		LOG_INFO("--------------------------------");
		LOG_INFO("          false sharing");
		LOG_INFO("--------------------------------");

		muggle_thread_t th1, th2;
		muggle_thread_create(&th1, increase, &data.a);
		muggle_thread_create(&th2, increase, &data.b);

		muggle_msleep(100);
		muggle_atomic_store(&ready, 1, muggle_memory_order_relaxed);

		muggle_thread_join(&th1);
		muggle_thread_join(&th2);

		// LOG_INFO("a=%d, b=%d", data.a, data.b);
	}

	// add cache line
	{
		ready = 0;

		bar_t data;
		memset(&data, 0, sizeof(data));

		LOG_INFO("--------------------------------");
		LOG_INFO("          cache line");
		LOG_INFO("--------------------------------");

		muggle_thread_t th1, th2;
		muggle_thread_create(&th1, increase, &data.a);
		muggle_thread_create(&th2, increase, &data.b);

		muggle_msleep(100);
		muggle_atomic_store(&ready, 1, muggle_memory_order_relaxed);

		muggle_thread_join(&th1);
		muggle_thread_join(&th2);

		// LOG_INFO("a=%d, b=%d", data.a, data.b);
	}

	return 0;
}
