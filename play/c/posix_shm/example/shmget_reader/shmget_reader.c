#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

typedef struct {
	uint32_t len;
	char buf[64];
} data_t;

int main()
{
	// get shm key
	const char *filepath = "/tmp";
	int proj_id = 5;
	key_t shm_key = ftok(filepath, proj_id);
	if (shm_key == -1) {
		fprintf(stderr, "failed ftok: path=%s, porj_id=%d\n", filepath,
				proj_id);
		exit(EXIT_FAILURE);
	}

	// open shm
	int shm_size = 0;
	int shm_id = shmget(shm_key, 0, 0);
	if (shm_id == -1) {
		fprintf(stderr, "failed open shm, key=%d\n", shm_key);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success get shm id: %d\n", shm_id);

	struct shmid_ds buf;
	if (shmctl(shm_id, IPC_STAT, &buf) == -1) {
		fprintf(stderr, "failed shmctl(IPC_STAT)\n");
		exit(EXIT_FAILURE);
	}

	shm_size = buf.shm_segsz;
	fprintf(stdout, "get shm size: %d\n", shm_size);

	// shm attach
	void *ptr = (void *)shmat(shm_id, NULL, 0);
	if (ptr == (void *)(-1)) {
		fprintf(stderr, "failed shmat\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success shmat, shm_id=%d\n", shm_id);

	data_t *data = (data_t *)ptr;
	fprintf(stdout, "read data: [%d] %s\n", data->len, data->buf);

	// shm detach
	if (shmdt(ptr) != 0) {
		fprintf(stderr, "failed shmdt\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success shmdt, shm_id=%d\n", shm_id);

	return 0;
}
