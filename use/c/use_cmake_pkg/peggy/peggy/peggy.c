#include <stdio.h>
#include "foo/foo.h"
#include "dave/dave.h"

int main()
{
	int s = 0;

	s = dave_add(5, 6);
	printf("peggy call: dave_add(5, 6) = %d\n", s);

	s = foo_add(5, 6);
	printf("peggy call: foo_add(5, 6) = %d\n", s);

	return 0;
}
