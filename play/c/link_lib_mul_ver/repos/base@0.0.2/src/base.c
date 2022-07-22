#include "base.h"

#include <stdio.h>

const char* base_version(char *buf, unsigned int bufsize)
{
#if defined(BASE_VER_MAJOR) && defined(BASE_VER_MINOR) && defined(BASE_VER_BUILD)
	snprintf(buf, bufsize, "%d.%d.%d", BASE_VER_MAJOR, BASE_VER_MINOR, BASE_VER_BUILD);
#else
	snprintf(buf, bufsize, "?.?.?");
#endif
	return buf;
}

int base_add(int a, int b)
{
	return a + b;
}
