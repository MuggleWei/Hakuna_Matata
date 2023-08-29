#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int get_idx(int i)
{
	int *p = (int *)malloc(sizeof(int) * N);
	for (int i = 0; i < N; i++) {
		p[i] = i;
	}
	free(p);

	// NOTE: BOOM!!!
	return *(int*)(p + i);
}

int main()
{
	srand(time(NULL));

	int v = get_idx(rand() % N);
	printf("%d\n", v);

	return 0;
}
