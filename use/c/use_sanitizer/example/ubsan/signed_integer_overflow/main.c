#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));

	int n = rand() % 2;
	n += 2;

	int32_t k = 0x7ffffffe;
	for (int32_t i = 0; i < n; i++) {
		printf("%d\n", k);
		k++;
	}

	return 0;
}
