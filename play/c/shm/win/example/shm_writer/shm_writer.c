#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <windows.h>

typedef struct {
	uint32_t len;
	char buf[64];
} data_t;

int main()
{
	DWORD shm_size = 1024;
	const char *name = "test_shm";

	HANDLE hMapFile;
	hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
								  shm_size, name);
	if (hMapFile != NULL) {
		fprintf(stdout, "success create file mapping\n");
	} else {
		fprintf(stderr, "failed create file mapping, errno=%d\n",
				GetLastError());

		hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
		if (hMapFile == NULL) {
			fprintf(stderr, "failed open file mapping: %s, errno=%d\n", name,
					GetLastError());
			exit(EXIT_FAILURE);
		} else {
			fprintf(stdout, "success open file mapping: %s\n", name);
		}
	}

	void *ptr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (ptr == NULL) {
		fprintf(stderr, "failed MapViewOfFile, errno=%d\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success map memory\n");

	// write data
	char buf[64];
	time_t ts = time(NULL);
	struct tm t;
	localtime_s(&t, &ts);
	snprintf(buf, sizeof(buf), "%d-%02d-%02dT%02d:%02d:%02d", t.tm_year + 1900,
			 t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	uint32_t len = strlen(buf);

	data_t *data = (data_t *)ptr;
	memset(data, 0, sizeof(*data));
	data->len = len;
	memcpy(data->buf, buf, len);

	fprintf(stdout, "write data: %s\n", data->buf);

	fprintf(stdout, "wait input to exit......\n");
	getch();

	if (!UnmapViewOfFile(ptr)) {
		fprintf(stderr, "failed UnmapViewOfFile\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success unmap memory\n");

	CloseHandle(hMapFile);
	fprintf(stdout, "close file mapping\n");

	return 0;
}