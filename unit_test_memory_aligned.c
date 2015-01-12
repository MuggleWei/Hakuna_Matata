#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "memory_aligned.h"

uintptr_t AlignedAlloc(size_t size, size_t alignment, void** alloc_ptr);
void AlignedTest();

int main(int argc, char* argv[])
{
	int i = 0;

	for (i=0; i<500; ++i)
	{
		AlignedTest();
	}

	fgetc(stdin);
	return 0;
}

void AlignedTest()
{
	int alloc_size			= 64;
	void* alloc_ptr			= (void*)0;
	uintptr_t aligned_ptr	= 0;
	size_t x				= POWER_OF_TWO_SIZEOF(19, alloc_size);
	unsigned char aligned1		= 0;
	unsigned char aligned2		= 0;
	uintptr_t alloc_address		= 0;
	uintptr_t aligned_address	= 0;

	aligned_ptr		= AlignedAlloc( x, alloc_size, &alloc_ptr );
	alloc_address	= (uintptr_t)alloc_ptr;
	aligned_address	= (uintptr_t)aligned_ptr;

	aligned1 = alloc_address & (alloc_size - 1) ? 0 : 1;
	aligned2 = aligned_address & (alloc_size - 1) ? 0 : 1;

	if (aligned2 == 0)
	{
		printf("aligned failed!\n");
	}
	else if ( (~aligned1) & aligned2 )
	{
		printf("aligned success!\n");
	}
	else
	{
		printf("can't judge\n");
	}

	free(alloc_ptr);
}