#include <stdlib.h>

int main()
{
	char *p = malloc(16);
	char *p2 = malloc(16);
	p[24] = 1;
	free(p2);
	free(p);

	return 0;
}
