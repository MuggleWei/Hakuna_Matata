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

int base_mul(int a, int b)
{
	char buf[32];
	const char *ver = base_version(buf, sizeof(buf));
	printf("invoke base_mul, base ver: %s\n", ver);

#if defined(BASE_VER_MAJOR) && defined(BASE_VER_MINOR) && defined(BASE_VER_BUILD)
	snprintf(buf, sizeof(buf), "%d.%d.%d", BASE_VER_MAJOR, BASE_VER_MINOR, BASE_VER_BUILD);
#else
	snprintf(buf, sizeof(buf), "?.?.?");
#endif
	printf("version macro: %s\n", buf);

	return a * b;
}
