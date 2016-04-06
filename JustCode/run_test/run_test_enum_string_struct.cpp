#include <stdio.h>
#include "enum_string_struct.h"

int main()
{
	int i = 0;

	for (i = 0; i < Enum1::Max; ++i)
	{
		printf("%s\t", Enum1::EnumString[i]);
	}
	printf("\n");

	for (i = 0; i < Enum2::Max; ++i)
	{
		printf("%s\t", Enum2::EnumString[i]);
	}
	printf("\n");

	return 0;
}