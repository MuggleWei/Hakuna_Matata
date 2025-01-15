#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	const char *name = "muggle_test_shm";

	int shm_fd = shm_open(name, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
	if (shm_fd == -1) {
		fprintf(stdout, "failed shm_open: %s\n", name);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success open shm, name=%s, fd=%d\n", name, shm_fd);

	if (shm_unlink(name) == -1) {
		fprintf(stdout, "failed shm_unlink\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success shm_unlink\n");

	return 0;
}
