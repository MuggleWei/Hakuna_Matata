#include <stdio.h>
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

	// create or open shm
	int shm_size = 32 * 1024 * 1024;
	// int flag_privilege = SHM_R | SHM_W;
	int flag_privilege = 0666;
	int shm_id =
		shmget(shm_key, shm_size, IPC_CREAT | IPC_EXCL | flag_privilege);
	if (shm_id != -1) {
		fprintf(stdout, "success create shm, key=%d\n", shm_key);
	} else {
		shm_id = shmget(shm_key, 0, 0);
		if (shm_id == -1) {
			fprintf(stderr, "failed get shm, key=%d\n", shm_key);
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
	}

	// shm attach
	void *ptr = (void *)shmat(shm_id, NULL, flag_privilege);
	if (ptr == (void *)(-1)) {
		fprintf(stderr, "failed shmat\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success shmat, shm_id=%d\n", shm_id);

	// shm detach
	if (shmdt(ptr) != 0) {
		fprintf(stderr, "failed shmdt\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success shmdt, shm_id=%d\n", shm_id);

	// shm rm
	if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
		fprintf(stderr, "failed shmctl(IPC_RMID)");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success rm shm, shm_id=%d", shm_id);

	return 0;
}
