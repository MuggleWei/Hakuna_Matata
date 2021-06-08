#include "bar.h"

#include <stdio.h>
#include "base.h"

void bar_run()
{
	printf("=========== bar_run start ===========\n"); 
	char buf[32];
	base_version(buf, sizeof(buf));
	printf("bar run with base version: %s\n", buf);

	int ret = base_add(5, 6);
	printf("base_add(5, 6) = %d\n", ret);
	printf("=========== bar_run end ===========\n"); 
}
