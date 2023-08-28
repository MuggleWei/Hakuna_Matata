#include <stdio.h>
#include <stdint.h>

int main()
{
	int32_t v = 0x7ffffff0;
	for (int32_t i = 0; i < 32; i++) {
		printf("%d\n", v);
		v++;
	}

	return 0;
}
