#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

typedef struct {
	uint32_t len;
	char buf[64];
} data_t;

void output_errmsg()
{
	char errmsg[512];
	strerror_r(errno, errmsg, sizeof(errmsg));
	fprintf(stderr, "errno: %d, errmsg: %s\n", errno, errmsg);
}

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

	// create or get shm
	int shm_size = 32 * 1024 * 1024;
	// int flag_privilege = SHM_R | SHM_W;
	int flag_privilege = 0666;
	int shm_id =
		shmget(shm_key, shm_size, IPC_CREAT | IPC_EXCL | flag_privilege);
	if (shm_id != -1) {
		fprintf(stdout, "success create shm, key=%d\n", shm_key);
	} else {
		fprintf(stderr, "failed create shm, key=%d, filepath=%s\n", shm_key,
				filepath);
		output_errmsg();

		shm_id = shmget(shm_key, 0, 0);
		if (shm_id == -1) {
			fprintf(stderr, "failed get shm, key=%d\n", shm_key);
			output_errmsg();
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

	// shm detach
	if (shmdt(ptr) != 0) {
		fprintf(stderr, "failed shmdt\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success shmdt, shm_id=%d\n", shm_id);

	// // shm rm
	// if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
	// 	fprintf(stderr, "failed shmctl(IPC_RMID)");
	// 	exit(EXIT_FAILURE);
	// }
	// fprintf(stdout, "success rm shm, shm_id=%d", shm_id);

	return 0;
}
