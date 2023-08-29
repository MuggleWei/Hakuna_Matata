#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	bool b = true;
	printf("%s\n", b ? "true" : "false");

	uint8_t u = 2;
	memcpy(&b, &u, sizeof(b));
	printf("%s\n", b ? "true" : "false");

	return 0;
}
