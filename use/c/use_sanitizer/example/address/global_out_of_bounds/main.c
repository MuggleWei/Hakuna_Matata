#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int arr[N] = {};

int main()
{
	srand(time(NULL));

	int i = rand() % 2 + N;
	// NOTE: BOOM!!!
	printf("%d\n", arr[i]);

	return 0;
}
