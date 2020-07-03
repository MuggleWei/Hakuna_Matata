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

	for (i = 0; i < Enum3::Max; ++i)
	{
		printf("%s\t", Enum3::EnumToString((Enum3::Enum)i));
	}
	printf("\n");

	Enum3::Enum e;
	e = Enum3::StringToEnum("A");
	if (e == Enum3::Enum::A)
	{
		printf("string to enum. A correct\n");
	}
	e = Enum3::StringToEnum("B");
	if (e == Enum3::Enum::B)
	{
		printf("string to enum. B correct\n");
	}
	e = Enum3::StringToEnum("c");
	if (e == Enum3::Enum::Max)
	{
		printf("string to enum. assert correct\n");
	}

	return 0;
}