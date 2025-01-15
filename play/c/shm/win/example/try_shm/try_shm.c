#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

int main()
{
	const int SIZE = 1024;
	const char *name = "test_shm";

	HANDLE hMapFile;
	hMapFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
								  SIZE, name);
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

	void *ptr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SIZE);
	if (ptr == NULL) {
		fprintf(stderr, "failed MapViewOfFile, errno=%d\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success map memory\n");

	if (!UnmapViewOfFile(ptr)) {
		fprintf(stderr, "failed UnmapViewOfFile\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "success unmap memory\n");

	CloseHandle(hMapFile);
	fprintf(stdout, "close file mapping\n");

	return 0;
}