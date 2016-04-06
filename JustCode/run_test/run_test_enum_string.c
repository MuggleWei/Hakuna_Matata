#include <stdio.h>
#include "enum_string.h"

int main()
{
	int i = 0;

	for (i = 0; i < E1_MAX; ++i)
	{
		printf("%s\t", EnumString1[i]);
	}
	printf("\n");

	for (i = 0; i < Enum2_MAX; ++i)
	{
		printf("%s\t", Enum2String[i]);
	}
	printf("\n");

	return 0;
}