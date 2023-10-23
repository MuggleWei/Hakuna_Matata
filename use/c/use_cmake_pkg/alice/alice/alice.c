#include "alice.h"
#include "foo/foo.h"

int alice_add(int a, int b)
{
	return foo_add(a, b);
}
