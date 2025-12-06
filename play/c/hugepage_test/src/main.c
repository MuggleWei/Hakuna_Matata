#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "gen/config.h"
#include "instrument_id.h"

#define ROUND_UP_POW_OF_2_MUL(x, n) (((x) + (n) - 1) & ~((n) - 1))
#define ALIGN_PAGE(n, page_size) ROUND_UP_POW_OF_2_MUL(n, page_size)
#define PAGE_SIZE_4K (4llu * 1024llu)
#define PAGE_SIZE_2M (2llu * 1024llu * 1024llu)
#define PAGE_SIZE_1G (1024llu * 1024llu * 1024llu)

typedef struct {
	int use_hugepage;
	int random_access;
	int prefetch;
	char page_size[16];
	char hugetlbfs_file[512];
} args_t;

typedef struct {
	char exchange[16];
	char instrument_id[64];
	float last_price;
	uint32_t qty;
	float turnover;
	float bid_price[5];
	float ask_price[5];
} record_t;

static int s_fd = -1;
static const char *s_exchanges[] = { "SHFE",  "DCE", "CZCE",
									 "CFFEX", "INE", "GFX" };

void output_usage(char *argv0, FILE *fp)
{
	fprintf(fp,
			"Usage: %s\n"
			"  -p, --hugepage    use huge page\n"
			"    * 0: without HUGE_TLB\n"
			"    * 1: use HUGE_TLB with mmap\n"
			"    * 2: use HUGE_TLB with hugetlbfs + mmap\n"
			"    * 3: use THP with madvise"
			"  -r, --random      random access data\n"
			"  -s, --pagesize    specific page size, run with '-p 1'\n"
			"  -f, --file        filepath in hugetlbfs\n"
			"  -P, --prefetch    prefetch memory\n"
			"",
			argv0);
}

void parse_args(int argc, char **argv, args_t *args)
{
	memset(args, 0, sizeof(*args));

	static struct option long_options[] = {
		{ "huagepage", required_argument, NULL, 'p' },
		{ "random", required_argument, NULL, 'r' },
		{ "pagesize", required_argument, NULL, 's' },
		{ "file", required_argument, NULL, 'f' },
		{ "prefetch", required_argument, NULL, 'P' },
		{ "help", required_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	int c;
	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "p:r:s:f:P:h", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h': {
			output_usage(argv[0], stdout);
			exit(EXIT_SUCCESS);
		} break;
		case 'p': {
			args->use_hugepage = atoi(optarg);
		} break;
		case 'r': {
			args->random_access = atoi(optarg);
		} break;
		case 's': {
			strncpy(args->page_size, optarg, sizeof(args->page_size) - 1);
		} break;
		case 'f': {
			strncpy(args->hugetlbfs_file, optarg,
					sizeof(args->hugetlbfs_file) - 1);
		} break;
		case 'P': {
			args->prefetch = atoi(optarg);
		} break;
		}
	}
}

/**
 * @brief alloc data with HUGE_TLB mmap
 */
void *alloc_datas_hugetlb_mmap(args_t *p_args, size_t n_bytes)
{
	// flags
	int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB;
	if (strcmp(p_args->page_size, "2MB") == 0) {
		flags |= MAP_HUGE_2MB;
	} else if (strcmp(p_args->page_size, "1GB") == 0) {
		flags |= MAP_HUGE_1GB;
	}

	// mmap
	void *datas = mmap(NULL, n_bytes, PROT_READ | PROT_WRITE, flags, -1, 0);
	if (datas == MAP_FAILED) {
		fprintf(stderr, "failed mmap, n_bytes=%llu, errno=%d\n",
				(unsigned long long)n_bytes, errno);
		return NULL;
	}

	return datas;
}

void *alloc_datas_hugetlbfs(args_t *p_args, size_t n_bytes)
{
	int flags = MAP_SHARED;
	if (p_args->hugetlbfs_file[0] == '\0') {
		fprintf(stderr, "use hugetlbfs+mmap without specify file\n");
		return NULL;
	}

	s_fd = open(p_args->hugetlbfs_file, O_CREAT | O_RDWR, 0755);
	if (s_fd == -1) {
		fprintf(stderr, "failed open %s\n", p_args->hugetlbfs_file);
		return NULL;
	}

	void *datas = mmap(NULL, n_bytes, PROT_READ | PROT_WRITE, flags, s_fd, 0);
	if (datas == MAP_FAILED) {
		fprintf(stderr, "failed mmap hugetlbfs, errno=%d\n", errno);
		close(s_fd);
		s_fd = -1;
		return NULL;
	}

	return datas;
}

void *alloc_datas(args_t *p_args, size_t n_bytes)
{
	// alloc datas
	void *datas = NULL;
	switch (p_args->use_hugepage) {
	case 1: {
		datas = alloc_datas_hugetlb_mmap(p_args, n_bytes);
	} break;
	case 2: {
		datas = alloc_datas_hugetlbfs(p_args, n_bytes);
	} break;
	case 3: {
		datas = aligned_alloc(PAGE_SIZE_4K, n_bytes);
		if (madvise(datas, n_bytes, MADV_HUGEPAGE) != 0) {
			fprintf(stderr,
					"failed madvise MADV_HUGEPAGE, n_bytes: %lu, errno: %d\n",
					(unsigned long)n_bytes, errno);
			free(datas);
			datas = NULL;
		}
	} break;
	default: {
		datas = aligned_alloc(PAGE_SIZE_4K, n_bytes);
	} break;
	}
	if (datas == NULL) {
		fprintf(stderr, "failed allocate memory\n");
		return NULL;
	}

	// prefetch
	if (p_args->prefetch != 0) {
#ifdef SUPPORT_MADV_POPULATE_WRITE
		if (madvise(datas, n_bytes, MADV_POPULATE_WRITE) != 0) {
			fprintf(stderr,
					"failed madivse MADV_POPULATE_WRITE, "
					"n_bytes: %lu, errno: %d\n",
					(unsigned long)n_bytes, errno);
		}
#else
		memset(datas, 0, n_bytes);
#endif
	}

	return datas;
}

int main(int argc, char *argv[])
{
	// parse args
	args_t args;
	parse_args(argc, argv, &args);
	fprintf(stdout, "----------------\n");
	fprintf(stdout, "use hugepage: %s\n", args.use_hugepage ? "on" : "off");
	fprintf(stdout, "random access: %s\n", args.random_access ? "on" : "off");
	fprintf(stdout, "page size: %s\n",
			args.page_size[0] != '\0' ? args.page_size : "default");
	fprintf(stdout, "hugetlbfs filepath: %s\n",
			args.hugetlbfs_file[0] != '\0' ? args.hugetlbfs_file : "(null)");
	fprintf(stdout, "prefetch: %s\n", args.prefetch ? "on" : "off");
	fprintf(stdout, "----------------\n");

	// init datas
	const size_t n_src_pos = 8 * PAGE_SIZE_2M / sizeof(uint32_t);
	const size_t n_bytes_src_pos = sizeof(uint32_t) * n_src_pos;
	const size_t n_dst_pos = 8 * PAGE_SIZE_2M / sizeof(uint32_t);
	const size_t n_bytes_dst_pos = sizeof(uint32_t) * n_dst_pos;
	size_t n_src = PAGE_SIZE_2M / sizeof(record_t);
	size_t n_bytes_src = sizeof(record_t) * n_src;
	size_t n_dst = 1024UL * 1024UL * 4UL;
	size_t n_bytes_dst = n_dst * sizeof(record_t);
	size_t n_pos = n_src_pos > n_dst_pos ? n_src_pos : n_dst_pos;
	size_t access_times = n_pos;

	// aligned bytes to page
	size_t n_total_bytes =
		n_bytes_src_pos + n_bytes_dst_pos + n_bytes_src + n_bytes_dst;
	fprintf(stdout,
			"number of datas:\n"
			"  - n_src_pos: %lu\n"
			"  - n_dst_pos: %lu\n"
			"  - n_src: %lu\n"
			"  - n_dst: %lu\n"
			"  - access_times: %lu\n",
			n_src_pos, n_dst_pos, n_src, n_dst, access_times);
	fprintf(stdout,
			"bytes of datas:\n"
			"  - n_bytes_src_pos: %llu\n"
			"  - n_bytes_dst_pos: %llu\n"
			"  - n_bytes_src: %llu\n"
			"  - n_bytes_dst: %llu\n"
			"  - total_bytes: %llu\n",
			(unsigned long long)n_bytes_src_pos,
			(unsigned long long)n_bytes_dst_pos,
			(unsigned long long)n_bytes_src, (unsigned long long)n_bytes_dst,
			(unsigned long long)n_total_bytes);
	if (strcmp(args.page_size, "2MB") == 0) {
		n_total_bytes = ALIGN_PAGE(n_total_bytes, PAGE_SIZE_2M);
	} else if (strcmp(args.page_size, "1GB") == 0) {
		n_total_bytes = ALIGN_PAGE(n_total_bytes, PAGE_SIZE_1G);
	} else {
		n_total_bytes = ALIGN_PAGE(n_total_bytes, PAGE_SIZE_2M);
	}
	fprintf(stdout, "total bytes after aligned: %llu\n",
			(unsigned long long)n_total_bytes);

	// allocate datas
	fprintf(stdout, "start allocate datas\n");
	void *datas = alloc_datas(&args, n_total_bytes);
	if (datas == NULL) {
		fprintf(stderr, "failed allocate datas\n");
		output_usage(argv[0], stderr);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "allocate datas completed\n");

	// split datas
	uint32_t *src_pos_arr = (uint32_t *)datas;
	uint32_t *dst_pos_arr = src_pos_arr + n_src_pos;
	record_t *src_datas = (record_t *)(dst_pos_arr + n_dst_pos);
	record_t *dst_datas = src_datas + n_src;
	srand(time(NULL));

	// fillup src datas
	fprintf(stdout, "fillup source datas\n");
	size_t n_exchange = sizeof(s_exchanges) / sizeof(s_exchanges[0]);
	size_t n_instrument_id =
		sizeof(s_instrument_ids) / sizeof(s_instrument_ids[0]);
	for (size_t i = 0; i < n_src; ++i) {
		record_t *data = src_datas + i;
		size_t idx_exchange = rand() % n_exchange;
		strncpy(data->exchange, s_exchanges[idx_exchange],
				sizeof(data->exchange) - 1);
		size_t idx_instrument_id = rand() % n_instrument_id;
		strncpy(data->instrument_id, s_instrument_ids[idx_instrument_id],
				sizeof(data->instrument_id) - 1);
		data->last_price = rand() % 1000 + 1000.0;
		data->qty = rand() % 1000 + 1000;
		data->turnover = data->last_price * data->qty;
		for (int i = 0; i < 5; ++i) {
			data->bid_price[i] = data->last_price - 1.0 * i;
			data->ask_price[i] = data->last_price + 1.0 * i;
		}
	}
	fprintf(stdout, "fillup source datas complated\n");

	// random access data
	fprintf(stdout, "prepare access datas\n");
	for (size_t i = 0; i < n_src_pos; ++i) {
		if (args.random_access) {
			src_pos_arr[i] = rand() % n_src;
		} else {
			src_pos_arr[i] = i % n_src;
		}
	}
	for (size_t i = 0; i < n_dst_pos; ++i) {
		if (args.random_access) {
			dst_pos_arr[i] = rand() % n_dst;
		} else {
			dst_pos_arr[i] = i % n_dst;
		}
	}
	fprintf(stdout, "prepare access datas completed\n");

	// run access
	fprintf(stdout, "access data begin\n");
	struct timespec start_ts, end_ts;
	clock_gettime(CLOCK_BOOTTIME, &start_ts);
	for (size_t i = 0; i < access_times; ++i) {
		uint32_t src_pos = src_pos_arr[i % n_src_pos];
		uint32_t dst_pos = dst_pos_arr[i % n_dst_pos];
		record_t *src_data = &src_datas[src_pos];
		record_t *dst_data = &dst_datas[dst_pos];
		memcpy(dst_data, src_data, sizeof(record_t));
	}
	clock_gettime(CLOCK_BOOTTIME, &end_ts);
	fprintf(stdout, "access data completed\n");

	int64_t elapsed = (end_ts.tv_sec - start_ts.tv_sec) * 1000LL +
					  ((end_ts.tv_nsec - start_ts.tv_nsec) / 1000000LL);
	fprintf(stdout, "elapsed ms: %lld\n", (long long)elapsed);

	// cleanup datas
	if (args.use_hugepage) {
		if (munmap(datas, n_total_bytes) == -1) {
			fprintf(stderr, "failed munmap, errno=%d\n", errno);
			exit(EXIT_FAILURE);
		}
		if (s_fd != -1) {
			close(s_fd);
		}
	} else {
		free(datas);
	}
	fprintf(stdout, "cleanup datas completed\n");

	return 0;
}
