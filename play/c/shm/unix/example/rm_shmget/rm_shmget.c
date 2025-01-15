#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

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
	int shm_id = shmget(shm_key, 0, 0);
	if (shm_id == -1) {
		fprintf(stderr, "failed open shm, key=%d\n", shm_key);
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success get shm id: %d\n", shm_id);

	// shm rm
	if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "failed shmctl(IPC_RMID)");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success rm shm, shm_id=%d\n", shm_id);

	return 0;
}

