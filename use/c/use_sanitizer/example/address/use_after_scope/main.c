#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *ptr = NULL;

int main()
{
	{
		int x = 0;
		ptr = &x;
	}

	*ptr = 5;
	printf("%d\n", *ptr);

	return 0;
}
