#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
	uint32_t len;
	char buf[64];
} data_t;

int main()
{
	const int SIZE = 32 * 1024 * 1024;
	const char *name = "muggle_test_shm";

	int shm_fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
	if (shm_fd == -1) {
		fprintf(stderr, "failed shm_open: %s\n", name);
		shm_fd = shm_open(name, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
		if (shm_fd == -1) {
			fprintf(stdout, "failed shm_open: %s\n", name);
			exit(EXIT_FAILURE);
		}
	} else {
		if (ftruncate(shm_fd, SIZE) == -1) {
			fprintf(stderr, "failed truncate shm\n");
			exit(EXIT_FAILURE);
		}
	}
	fprintf(stdout, "success open shm, name=%s, fd=%d\n", name, shm_fd);

	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "failed mmap\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success mmap\n");

	// write data
	char buf[64];
	time_t ts = time(NULL);
	struct tm t;
	localtime_r(&ts, &t);
	snprintf(buf, sizeof(buf), "%d-%02d-%02dT%02d:%02d:%02d", t.tm_year + 1900,
			 t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	uint32_t len = strlen(buf);

	data_t *data = (data_t *)ptr;
	memset(data, 0, sizeof(*data));
	data->len = len;
	memcpy(data->buf, buf, len);

	fprintf(stdout, "write data: %s\n", data->buf);

	// write dummy data and watch size change in /dev/shm/${name}
	int remain_size = SIZE - sizeof(uint32_t) - len;
	int n = remain_size / sizeof(data_t);
	data_t *p = data + 1;
	for (int i = 0; i < n; ++i) {
		memcpy(p, data, sizeof(data_t));
		++p;
	}

	if (munmap(ptr, SIZE) == -1) {
		fprintf(stdout, "failed mnump\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success munmap\n");

	close(shm_fd);

	// if (shm_unlink(name) == -1) {
	//     fprintf(stdout, "failed shm_unlink\n");
	//     exit(EXIT_FAILURE);
	// }
	// fprintf(stdout, "success shm_unlink\n");

	return 0;
}
