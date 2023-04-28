#include "str.h"
#include <stdlib.h>
#include <string.h>

char* foo_str_join(const char *s1, const char *s2)
{
	size_t l1 = strlen(s1);
	size_t l2 = strlen(s2);
	char *s = (char*)malloc(l1 + l2 + 1);
	memcpy(s, s1, l1);
	memcpy(s + l1, s2, l2);
	s[l1 + l2] = '\0';

	return s;
}
