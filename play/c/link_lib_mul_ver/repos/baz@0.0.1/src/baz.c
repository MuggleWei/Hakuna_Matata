#include "baz.h"

#include <stdio.h>
#include "base.h"

void baz_run()
{
	printf("=========== baz_run start ===========\n"); 
	char buf[32];
	base_version(buf, sizeof(buf));
	printf("baz run with base version: %s\n", buf);

	int ret = base_mul(5, 6);
	printf("base_mul(5, 6) = %d\n", ret);
	printf("=========== baz_run end ===========\n"); 
}
