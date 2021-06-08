#include "foo.h"

#include <stdio.h>
#include "base.h"

void foo_run()
{
	printf("=========== foo_run start ===========\n"); 
	char buf[32];
	base_version(buf, sizeof(buf));
	printf("foo run with foo version: %s\n", buf);
	printf("=========== foo_run end ===========\n"); 
}
