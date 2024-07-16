#include "foo_module.h"
#include "sodium.h"

void foo_module_run()
{
	if (sodium_init() != 0) {
		fprintf(stderr, "failed sodium init\n");
		return;
	}

	fprintf(stdout, "sodium version: %s\n", sodium_version_string());
}
