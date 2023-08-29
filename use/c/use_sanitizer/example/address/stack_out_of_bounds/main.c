#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int main()
{
	srand(time(NULL));

	int arr[N];
	arr[0] = 0;

	int i = rand() % 2 + N;
	// NOTE: BOOM!!!
	printf("%d\n", arr[i]);

	return 0;
}
