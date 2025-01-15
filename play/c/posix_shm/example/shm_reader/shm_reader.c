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
	const int SIZE = 1024;
	const char *name = "muggle_test_shm";

	int shm_fd = shm_open(name, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
	if (shm_fd == -1) {
		fprintf(stdout, "failed shm_open: %s\n", name);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success open shm, name=%s, fd=%d\n", name, shm_fd);

	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "failed mmap\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success mmap\n");

	data_t *data = (data_t *)ptr;
	fprintf(stdout, "read data: [%d] %s\n", data->len, data->buf);

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
