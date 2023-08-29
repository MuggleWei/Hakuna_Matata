#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 100

int main()
{
	srand(time(NULL));

	int *arr = (int*)malloc(sizeof(int) * N);
	memset(arr, 0, sizeof(int));

	int i = rand() % (N - 1) + 1;
	// NOTE: BOOM!!!
	if (arr[i]) {
		printf("%d: %d\n", i, arr[i]);
	}

	free(arr);

	return 0;
}
