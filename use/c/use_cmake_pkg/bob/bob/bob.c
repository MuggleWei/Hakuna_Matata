#include <stdio.h>
#include "foo/foo.h"

int main()
{
	int s = foo_add(5, 6);
	printf("bob call: foo_add(5, 6) = %d\n", s);

	return 0;
}
