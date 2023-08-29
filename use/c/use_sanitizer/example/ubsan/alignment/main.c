#include <stdio.h>
#include <stdint.h>

typedef struct foo {
	union {
		uint64_t u64;
		uint16_t u16_arr[4];
	};
} foo_t;

int main()
{
	foo_t f;
	f.u64 = 0x0001000200030004;

	char *p = (char *)&f;
	foo_t *pf = (foo_t *)(p + 2);
	printf("%u", pf->u16_arr[0]);

	return 0;
}
