#include "foo.h"
#include <stdio.h>
#include "utils/bar.h"

void foo_print()
{
	printf("hello foo\n");
	bar_print();
}
