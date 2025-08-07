#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <sys/mman.h>
#include <errno.h>

typedef struct {
	int use_hugepage;
	int random_access;
	char page_size[16];
} args_t;

typedef union {
	struct {
		uint32_t update_cnt;
		time_t last_update_ts;
	};
	char buf[64];
} record_t;

void output_usage(char *argv0, FILE *fp)
{
	fprintf(fp,
			"Usage: %s\n"
			"  -p, --hugepage    use huge page\n"
			"  -r, --random      random access data\n"
			"  -s, --pagesize    specific page size\n",
			argv0);
}

void parse_args(int argc, char **argv, args_t *args)
{
	static struct option long_options[] = {
		{ "huagepage", required_argument, NULL, 'p' },
		{ "random", required_argument, NULL, 'r' },
		{ "pagesize", required_argument, NULL, 's' },
		{ "help", required_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	int c;
	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "p:r:s:h", long_options, &option_index);
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
		}
	}
}

int main(int argc, char *argv[])
{
	// parse args
	args_t args;
	parse_args(argc, argv, &args);
	fprintf(stdout, "use hugepage: %s\n", args.use_hugepage ? "yes" : "no");
	fprintf(stdout, "random access: %s\n", args.random_access ? "yes" : "no");
	fprintf(stdout, "page size: %s\n",
			args.page_size[0] != '\0' ? args.page_size : "default");

	// init datas
	size_t n = 1024UL * 1024UL * 16UL;
	size_t n_bytes = n * sizeof(record_t);
	record_t *datas = NULL;
	fprintf(stdout, "start allocate datas\n");
	if (args.use_hugepage) {
		int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB;
		if (args.page_size[0] != '\0') {
			if (strcmp(args.page_size, "2MB") == 0) {
				flags |= MAP_HUGE_2MB;
			} else if (strcmp(args.page_size, "1GB") == 0) {
				flags |= MAP_HUGE_1GB;
			}
		}
		datas = (record_t *)mmap(NULL, n_bytes, PROT_READ | PROT_WRITE, flags,
								 -1, 0);
		if (datas == MAP_FAILED) {
			fprintf(stderr, "failed mmap, errno=%d\n", errno);
			exit(EXIT_FAILURE);
		}
	} else {
		datas = (record_t *)malloc(n_bytes);
		if (datas == NULL) {
			fprintf(stderr, "failed allocate memory\n");
			exit(EXIT_FAILURE);
		}
	}
	fprintf(stdout, "allocate datas completed\n");

	// random access data
	fprintf(stdout, "run access datas\n");
	struct timespec start_ts, end_ts;
	srand(time(NULL));
	clock_gettime(CLOCK_BOOTTIME, &start_ts);
	for (size_t i = 0; i < 500000; ++i) {
		int pos = 0;
		if (args.random_access) {
			pos = rand() % n;
		} else {
			pos = i % n;
		}
		++datas[pos].update_cnt;
		datas[pos].last_update_ts = time(NULL);
	}
	clock_gettime(CLOCK_BOOTTIME, &end_ts);

	int64_t elapsed = (end_ts.tv_sec - start_ts.tv_sec) * 1000LL +
					  ((end_ts.tv_nsec - start_ts.tv_nsec) / 1000000LL);
	fprintf(stdout, "access datas completed\n");
	fprintf(stdout, "elapsed ms: %lld\n", (long long)elapsed);

	// cleanup datas
	fprintf(stdout, "start cleanup datas\n");
	if (args.use_hugepage) {
		if (munmap(datas, n_bytes) == -1) {
			fprintf(stderr, "failed munmap, errno=%d\n", errno);
			exit(EXIT_FAILURE);
		}
	} else {
		free(datas);
	}
	fprintf(stdout, "cleanup datas completed\n");

	return 0;
}
