#include "foo.h"
#include <stdio.h>
#include "base/base.h"

void foo_say()
{
	printf("foo say: base version is: %s\n", base_ver());
}
