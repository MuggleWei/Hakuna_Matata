#include "foo.h"
#include <stdio.h>

#define FOO_VERSION "1.2.0"

void foo_say_hello(const char *name)
{
	fprintf(stdout, "hello %s, it's foo %s\n", name, FOO_VERSION);
}
