#define MUGGLE_HOLD_LOG_MACRO 1
#include "muggle/c/muggle_c.h"
#include "common/config.h"
#include "common/utils.h"
#include "common/matrix.h"
#include "common/simd/matrix_sse4.h"
#include "common/simd/matrix_avx2.h"
#include "common/simd/matrix_neon.h"

enum {
	OPT_VAL_INSTRUCT = 1000,
	OPT_VAL_METHOD,
};

typedef struct {
	char instruct[8];
	char method[8];
	int total_n;
	int warnup_n;
} example_matrix_cfg_t;

bool parse_args(int argc, char **argv, example_matrix_cfg_t *cfg)
{
	const char *str_usage =
		"Usage: %s <options>\n"
		"    -h, --help      show man info\n"
		"      , --instruct  instruct: scalar|sse4|avx2|neon\n"
		"      , --method    method: add|mul\n"
		"    -n, --total_n   total number of datas\n"
		"    -w, --warnup_n  warnup number of datas\n"
		"";

	memset(cfg, 0, sizeof(*cfg));
	strncpy(cfg->instruct, "scalar", sizeof(cfg->instruct) - 1);
	strncpy(cfg->method, "add", sizeof(cfg->method) - 1);
	cfg->total_n = 100000;
	cfg->warnup_n = 20000;

	int c = 0;
	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "help", no_argument, NULL, 'h' },
			{ "instruct", required_argument, NULL, OPT_VAL_INSTRUCT },
			{ "method", required_argument, NULL, OPT_VAL_METHOD },
			{ "total_n", required_argument, NULL, 'n' },
			{ "warnup_n", required_argument, NULL, 'w' },
			{ NULL, 0, NULL, 0 }
		};
		c = getopt_long(argc, argv, "hn:w:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			fprintf(stdout, "%s\n", str_usage);
			exit(EXIT_SUCCESS);
		} break;
		case OPT_VAL_INSTRUCT: {
			strncpy(cfg->instruct, optarg, sizeof(cfg->instruct) - 1);
		} break;
		case OPT_VAL_METHOD: {
			strncpy(cfg->method, optarg, sizeof(cfg->method) - 1);
		} break;
		case 'n': {
			cfg->total_n = atoi(optarg);
		} break;
		case 'w': {
			cfg->warnup_n = atoi(optarg);
		} break;
		default: {
			fprintf(stderr, "unrecognized opt: %s\n", optarg);
		} break;
		}
	}

	fprintf(stdout,
			"-------- input args --------\n"
			"instruct: %s\n"
			"method: %s\n"
			"total_n: %d\n"
			"warnup_n: %d\n"
			"----------------------------\n",
			cfg->instruct, cfg->method, cfg->total_n, cfg->warnup_n);

	return true;
}

void run_scalar_add(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		matrix4_add(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		matrix4_add(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 scalar add, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

void run_scalar_mul(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		matrix4_mul(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		matrix4_mul(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 scalar mul, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

#if SIMD_HAS_SSE4

void run_sse4_add(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_sse4_matrix4_add(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_sse4_matrix4_add(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 sse add, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

void run_sse4_mul(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_sse4_matrix4_mul(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_sse4_matrix4_mul(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 sse mul, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

#endif /* if SIMD_HAS_SSE4 */

#if SIMD_HAS_AVX2

void run_avx2_add(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_avx2_matrix4_add(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_avx2_matrix4_add(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 avx add, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

void run_avx2_mul(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_avx2_matrix4_mul(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_avx2_matrix4_mul(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 avx mul, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

#endif /* if SIMD_HAS_AVX2 */

#if SIMD_HAS_NEON

void run_neon_add(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_neon_matrix4_add(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_neon_matrix4_add(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 neon add, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

void run_neon_mul(example_matrix_cfg_t *cfg, matrix4_t *mat_arr)
{
	// warnup
	for (int i = 0; i < cfg->warnup_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_neon_matrix4_mul(a, b, c);
	}

	// run
	muggle_time_counter_t tc;
	muggle_time_counter_init(&tc);
	muggle_time_counter_start(&tc);
	for (int i = cfg->warnup_n; i < cfg->total_n; ++i) {
		matrix4_t *a = mat_arr + i * 3 + 0;
		matrix4_t *b = mat_arr + i * 3 + 1;
		matrix4_t *c = mat_arr + i * 3 + 2;
		simd_neon_matrix4_mul(a, b, c);
	}
	muggle_time_counter_end(&tc);

	int64_t elapsed_ns = muggle_time_counter_interval_ns(&tc);
	double elapsed_ms = elapsed_ns / 1000000.0;
	LOG_INFO("matrix 4x4 neon mul, n=%d, warnup=%d, elapsed=%.3f ms",
			 cfg->total_n, cfg->warnup_n, elapsed_ms);
}

#endif /* if SIMD_HAS_NEON */

int main(int argc, char *argv[])
{
	// initialize config
	example_matrix_cfg_t cfg;
	if (!parse_args(argc, argv, &cfg)) {
		fprintf(stderr, "failed parse arguments\n");
		exit(EXIT_FAILURE);
	}

	// initialize log
	if (muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL) != 0) {
		fprintf(stderr, "failed init log\n");
		exit(EXIT_FAILURE);
	}

	// prepared datas
	matrix4_t *mat_arr = (matrix4_t *)common_aligned_alloc(
		64, sizeof(matrix4_t) * cfg.total_n * 3);
	if (mat_arr == NULL) {
		LOG_ERROR("failed allocate datas");
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));
	for (int i = 0; i < cfg.total_n * 3; ++i) {
		for (int j = 0; j < 16; ++j) {
			mat_arr[i].vals[j] = rand() % 100 - 50.0f;
		}
	}

	// run
	if (strcmp(cfg.instruct, "scalar") == 0) {
		if (strcmp(cfg.method, "add") == 0) {
			run_scalar_add(&cfg, mat_arr);
		} else if (strcmp(cfg.method, "mul") == 0) {
			run_scalar_mul(&cfg, mat_arr);
		}
	} else if (strcmp(cfg.instruct, "sse4") == 0) {
#if SIMD_HAS_SSE4
		if (strcmp(cfg.method, "add") == 0) {
			run_sse4_add(&cfg, mat_arr);
		} else if (strcmp(cfg.method, "mul") == 0) {
			run_sse4_mul(&cfg, mat_arr);
		}
#else
		LOG_ERROR("not support SSE4");
#endif
	} else if (strcmp(cfg.instruct, "avx2") == 0) {
#if SIMD_HAS_AVX2
		if (strcmp(cfg.method, "add") == 0) {
			run_avx2_add(&cfg, mat_arr);
		} else if (strcmp(cfg.method, "mul") == 0) {
			run_avx2_mul(&cfg, mat_arr);
		}
#else
		LOG_ERROR("not support AVX2");
#endif
	} else if (strcmp(cfg.instruct, "neon") == 0) {
#if SIMD_HAS_NEON
		if (strcmp(cfg.method, "add") == 0) {
			run_neon_add(&cfg, mat_arr);
		} else if (strcmp(cfg.method, "mul") == 0) {
			run_neon_mul(&cfg, mat_arr);
		}
#else
		LOG_ERROR("not support NEON");
#endif
	}

	// cleanup
	common_aligned_free(mat_arr);

	return 0;
}
