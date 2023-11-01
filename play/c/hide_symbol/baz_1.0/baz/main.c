#include <stdio.h>
#include "foo/foo.h"
#include "bar/bar.h"
#include "bob/bob.h"

#define BAZ_VERSION "1.0.0"

int main()
{
	fprintf(stdout, "baz[%s] call | ", BAZ_VERSION);
	foo_say_hello("world");

	bar_say_hello("world");
	bob_say_hello("world");

	return 0;
}
