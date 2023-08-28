#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *p = (int*)malloc(sizeof(int));
	*p = 5;
	printf("%d\n", *p);

	return 0;
}
