#include "bob.h"
#include <stdio.h>
#include "foo/foo.h"

#define BOB_VERSION "1.0.0"

void bob_say_hello(const char *name)
{
	fprintf(stdout, "bob[%s] call | ", BOB_VERSION);
	foo_say_hello(name);
}
