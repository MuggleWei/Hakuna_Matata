#include "bar.h"
#include <stdio.h>
#include "base/base.h"

void bar_say()
{
	printf("bar.base say: %s\n", base_say_hello());
	printf("bar say: base version is: %s\n", base_ver());
}
