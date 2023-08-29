#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

static const int N = 5;

void fillup(int *p, int n)
{
	memset(p, 0, sizeof(int) * n);
	for (int i = 0; i < n; i++) {
		p[i] = rand() % 16;
	}
}

int cal_sum(int *p, int n)
{
	int sum = 0;
	for (int i = 0; i < n; i++) {
		// NOTE: out of bounds
		if (p[i + 1] > 8) {
			sum += p[i];
		} else {
			sum -= p[i];
		}
	}

	return sum;
}

int local_array()
{
	int f[N];
	fillup(f, N);
	return cal_sum(f, N);
}

int dynamic_array()
{
	int n = rand() % N + 1;
	int *p = malloc(n * sizeof(int));
	fillup(p, n);
	int sum = cal_sum(p, n);
	free(p);

	return sum;
}

int main()
{
	srand(time(NULL));

	int sum = 0;
	sum = local_array();
	printf("local array sum: %d\n", sum);

	// sum = dynamic_array();
	// printf("dynamic array sum: %d\n", sum);

	return true;
}
