#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

typedef struct {
	uint32_t len;
	char buf[64];
} data_t;

int main()
{
	const char *name = "test_shm";

	HANDLE hMapFile;
	hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
	if (hMapFile == NULL) {
		fprintf(stderr, "failed open file mapping: %s, errno=%d\n", name,
				GetLastError());
		exit(EXIT_FAILURE);
	} else {
		fprintf(stdout, "success open file mapping: %s\n", name);
	}

	void *ptr =
		MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(data_t));
	if (ptr == NULL) {
		fprintf(stderr, "failed MapViewOfFile, errno=%d\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success map memory\n");

	data_t *data = (data_t *)ptr;
	fprintf(stdout, "read data: [%d] %s\n", data->len, data->buf);

	if (!UnmapViewOfFile(ptr)) {
		fprintf(stderr, "failed UnmapViewOfFile\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success unmap memory\n");

	CloseHandle(hMapFile);
	fprintf(stdout, "close file mapping\n");

	return 0;
}