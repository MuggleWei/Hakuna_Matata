#include "bar.h"
#include <stdio.h>
#include "foo/foo.h"

#define BAR_VERSION "1.0.0"

void bar_say_hello(const char *name)
{
	fprintf(stdout, "bar call foo, it's bar %s\n", BAR_VERSION);
	foo_say_hello(name);
}
