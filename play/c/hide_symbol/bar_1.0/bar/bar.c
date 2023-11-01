#include "bar.h"
#include <stdio.h>
#include "foo/foo.h"

void bar_say_hello(const char *name)
{
	fprintf(stdout, "bar[%s] call | ", BAR_VERSION);
	foo_say_hello(name);
}
