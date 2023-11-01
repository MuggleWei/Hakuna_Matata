#include "foo.h"
#include <stdio.h>

void foo_say_hello(const char *name)
{
	fprintf(stdout, "hello %s, it's foo %s\n", name, FOO_VERSION);
}
