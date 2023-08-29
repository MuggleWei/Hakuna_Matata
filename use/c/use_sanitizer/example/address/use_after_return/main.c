#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int *ptr = NULL;

__attribute__((noinline))
void set_ptr(int idx)
{
	int arr[N] = {};
	for (int i = 0; i < N; i++) {
		arr[i] = i * 2;
	}

	ptr = &arr[idx];
}

int main()
{
	srand(time(NULL));

	set_ptr(rand() % N);

	// NOTE: BOOM!!!
	printf("%d\n", *ptr);

	return 0;
}

