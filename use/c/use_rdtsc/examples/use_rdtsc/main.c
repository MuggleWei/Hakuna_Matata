#include "muggle/c/muggle_c.h"
#include <inttypes.h>

#if __linux__ && defined(__x86_64__)

	#include <x86intrin.h>

int64_t read_cpu_hz()
{
	FILE *file;
	char buffer[1024];
	double mhz = 0.0;

	file = fopen("/proc/cpuinfo", "r");
	if (file == NULL) {
		perror("Error opening /proc/cpuinfo");
		return EXIT_FAILURE;
	}

	while (fgets(buffer, sizeof(buffer), file)) {
		if (strncmp(buffer, "cpu MHz", 7) == 0) {
			mhz = atof(strchr(buffer, ':') + 1);
			break;
		}
	}

	fclose(file);

	return mhz * 1000000ll;
}

int main()
{
	uint64_t cpu_cycles_per_sec = (uint64_t)read_cpu_hz();
	if (cpu_cycles_per_sec == 0) {
		fprintf(stderr, "failed read cpu hz");
		exit(EXIT_FAILURE);
	}

	uint64_t start = __rdtsc();
	muggle_msleep(1);
	uint64_t end = __rdtsc();

	uint64_t cycles = end - start;
	double sec = (double)cycles / cpu_cycles_per_sec;
	fprintf(stdout, "CPU cycles per second: %" PRIu64 "\n", cpu_cycles_per_sec);
	fprintf(stdout, "elapsed CPU cycles: %" PRIu64 "\n", cycles);
	fprintf(stdout, "elapsed time: %f s\n", sec);

	return 0;
}

#else

int main()
{
	fprintf(stderr, "this example only support linux with x86_64\n");
	exit(EXIT_FAILURE);
}

#endif
