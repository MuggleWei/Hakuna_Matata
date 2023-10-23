#include <stdio.h>
#include "alice/alice.h"
#include "foo/foo.h"

int main()
{
	int s = 0;

	s = alice_add(5, 6);
	printf("pat call: alice_add(5, 6) = %d\n", s);

	s = foo_add(5, 6);
	printf("pat call: foo_add(5, 6) = %d\n", s);

	return 0;
}
