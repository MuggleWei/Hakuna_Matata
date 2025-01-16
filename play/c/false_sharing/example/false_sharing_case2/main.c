#include "muggle/c/muggle_c.h"

#if DATA_ALIGN_SIZE_128
typedef struct {
	uint32_t increase1;
	uint32_t increase2;
	MUGGLE_STRUCT_CACHE_LINE_X2_PADDING(0);
	uint32_t val1;
	MUGGLE_STRUCT_CACHE_LINE_X2_PADDING(1);
	uint32_t val2;
} data_t;
#elif DATA_ALIGN_SIZE_64
typedef struct {
	uint32_t increase1;
	uint32_t increase2;
	MUGGLE_STRUCT_CACHE_LINE_PADDING(0);
	uint32_t val1;
	MUGGLE_STRUCT_CACHE_LINE_X2_PADDING(0);
	uint32_t val2;
} data_t;
#else
typedef struct {
	uint32_t increase1;
	uint32_t increase2;
	uint32_t val1;
	MUGGLE_STRUCT_CACHE_LINE_X2_PADDING(0);
	uint32_t val2;
} data_t;
#endif

typedef struct {
	int cpu;
	uint32_t *pval;
	uint32_t *pincr;
	int64_t elapsed;
	uint32_t *ready;
} thread_data_t;

muggle_thread_ret_t atomic_increase(void *args)
{
	thread_data_t *data = (thread_data_t *)args;
	uint32_t *pval = data->pval;
	uint32_t *pincr = data->pincr;

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
		muggle_atomic_fetch_add(pval, *pincr, muggle_memory_order_relaxed);
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
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <cpu-1> <cpu-2>\n", argv[0]);
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

	MUGGLE_ALIGNAS(MUGGLE_CACHE_LINE_SIZE) data_t data;
	MUGGLE_ALIGNAS(MUGGLE_CACHE_LINE_SIZE) thread_data_t th_datas[2];

	th_datas[0].cpu = atoi(argv[1]);
	th_datas[0].pval = &data.val1;
	th_datas[0].pincr = &data.increase1;
	th_datas[0].elapsed = 0;
	th_datas[0].ready = &ready;

	th_datas[1].cpu = atoi(argv[2]);
	th_datas[1].pval = &data.val2;
	th_datas[1].pincr = &data.increase2;
	th_datas[1].elapsed = 0;
	th_datas[1].ready = &ready;

	run(th_datas, n, &ready);

#if DATA_ALIGN_SIZE_128
	fprintf(stdout, "data interval x2 cache line\n");
#elif DATA_ALIGN_SIZE_64
	fprintf(stdout, "data interval cache line\n");
#else
	fprintf(stdout, "data without interval \n");
#endif
	for (int i = 0; i < n; ++i) {
		fprintf(stdout, "cpu: %d, elapsed: %lld ns\n", th_datas[i].cpu,
				(long long)th_datas[i].elapsed);
	}

	return 0;
}
