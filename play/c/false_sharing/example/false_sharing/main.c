#include "muggle/c/muggle_c.h"

#if DATA_ALIGN_SIZE_128
typedef union {
	muggle_sync_t val;
	MUGGLE_STRUCT_CACHE_LINE_X2_PADDING(0);
} data_t;
#elif DATA_ALIGN_SIZE_64
typedef union {
	muggle_sync_t val;
	MUGGLE_STRUCT_CACHE_LINE_PADDING(0);
} data_t;
#else
typedef struct {
	muggle_sync_t val;
} data_t;
#endif

typedef struct {
	int cpu;
	muggle_sync_t *pval;
	int64_t elapsed;
	uint32_t *ready;
} thread_data_t;

muggle_thread_ret_t atomic_increase(void *args)
{
	thread_data_t *data = (thread_data_t *)args;
	muggle_sync_t *pval = data->pval;

	// bind cpu
	muggle_cpu_mask_t mask;
	muggle_cpu_mask_zero(&mask);
	muggle_cpu_mask_set(&mask, data->cpu);
	if (muggle_cpu_set_thread_affinity(0, &mask) != 0) {
		fprintf(stderr, "failed bind CPU #%d\n", data->cpu);
	}

	// wait to start
	while (muggle_atomic_load(data->ready, muggle_memory_order_relaxed) == 0) {
	}

	// run atomic increase
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (uint32_t i = 0; i < (1 << 20); ++i) {
		muggle_atomic_fetch_add(pval, 1, muggle_memory_order_relaxed);
	}
	muggle_time_counter_end(&tc);

	data->elapsed = muggle_time_counter_interval_ns(&tc);

	return 0;
}

void run(thread_data_t *th_datas, int n, uint32_t *ready)
{
	muggle_thread_t *th_arr =
		(muggle_thread_t *)malloc(sizeof(muggle_thread_t) * n);

	for (int i = 0; i < n; ++i) {
		muggle_thread_create(&th_arr[i], atomic_increase, &th_datas[i]);
	}

	muggle_msleep(100);
	muggle_atomic_store(ready, 1, muggle_memory_order_relaxed);

	for (int i = 0; i < n; ++i) {
		muggle_thread_join(&th_arr[i]);
	}

	free(th_arr);
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <cpu-1> <cpu-2> [cpu-3] ... [cpu-N]\n",
				argv[0]);
		fprintf(stderr, "NOTE: try cpu nodes in same or different NUMA\n");
		fprintf(stderr,
				"e.g.\n"
				"  perf stat -e "
				"instructions,cycles,cache-misses,L1-dcache-load-misses,"
				"LLC-load-misses,node-load-misses %s 0 1\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	int n = argc - 1;
	uint32_t ready = 0;

#if MUGGLE_C_HAVE_ALIGNED_ALLOC
	data_t *datas =
		(data_t *)aligned_alloc(MUGGLE_CACHE_LINE_X2_SIZE, sizeof(data_t) * n);
	thread_data_t *th_datas = (thread_data_t *)aligned_alloc(
		MUGGLE_CACHE_LINE_X2_SIZE, sizeof(thread_data_t) * n);
#else
	data_t *datas = (data_t *)malloc(sizeof(data_t) * n);
	thread_data_t *th_datas = malloc(sizeof(thread_data_t) * n);
#endif
	for (int i = 0; i < n; ++i) {
		th_datas[i].cpu = atoi(argv[i + 1]);
		th_datas[i].pval = &datas[i].val;
		th_datas[i].elapsed = 0;
		th_datas[i].ready = &ready;
	}

	run(th_datas, n, &ready);

#if DATA_ALIGN_SIZE_128
	fprintf(stdout, "data align x2 cache line\n");
#elif DATA_ALIGN_SIZE_64
	fprintf(stdout, "data align cache line\n");
#else
	fprintf(stdout, "data without align\n");
#endif
	for (int i = 0; i < n; ++i) {
		fprintf(stdout, "cpu: %d, elapsed: %lld ns\n", th_datas[i].cpu,
				(long long)th_datas[i].elapsed);
	}

	free(th_datas);
	free(datas);

	return 0;
}
