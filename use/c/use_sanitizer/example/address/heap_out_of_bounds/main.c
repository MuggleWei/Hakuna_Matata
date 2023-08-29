#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int main()
{
	srand(time(NULL));

	int n = rand() % N + 5;
	int *arr = malloc(n * sizeof(int));

	arr[0] = 0;
	int *p = arr + n;
	// NOTE: BOOM!!!
	printf("%d\n", *p);

	free(arr);

	return 0;
}
