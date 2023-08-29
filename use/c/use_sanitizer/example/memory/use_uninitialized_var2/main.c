#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

typedef struct base {
	uint64_t u64;
} base_t;

typedef struct foo {
	base_t b;
	uint64_t u64;
	uint32_t u32;
	int32_t i32;
} foo_t;

void output_foo(foo_t *p)
{
	printf("u64=%llu, u32=%u, i32=%d\n", (unsigned long long)p->u64, p->u32,
		   p->i32);
}

void output_base(base_t *p)
{
	printf("u64=%llu\n", (unsigned long long)p->u64);
}

int main()
{
	srand(time(NULL));

	base_t *p = (base_t *)malloc(sizeof(foo_t));
	p->u64 = rand() % 16;

	output_base(p);

	// NOTE: can't detect uninitialized part
	output_foo((foo_t *)p);

	return 0;
}
