#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "memory_aligned.h"

#define ALLOC_BLOCK_SIZE	16
#define ALIGNMENT			32

void AlignedTest();

int main(int argc, char* argv[])
{
	int i = 0;
	srand((unsigned int)time(NULL));

	for (i=0; i<500; ++i)
	{
		AlignedTest();
	}

	fgetc(stdin);
	return 0;
}

void AlignedTest()
{
	void* alloc_ptr		= NULL;
	void* aligned_ptr	= 0;
	bool aligned_flag	= false;
	int alloc_size		= ALLOC_BLOCK_SIZE * (1 + rand()%10) + (rand() % ALLOC_BLOCK_SIZE);
	int real_alloc_size	= POWER_OF_TWO_SIZEOF(alloc_size, ALLOC_BLOCK_SIZE);

	aligned_ptr = (void*)AlignedAlloc( ALIGNMENT, real_alloc_size, &alloc_ptr );
	aligned_flag = ( (((uintptr_t)aligned_ptr) & (ALIGNMENT - 1)) == 0 );

	if ( !aligned_flag )
	{
		printf("aligned failed!\n");
	}
	else if ( aligned_ptr == alloc_ptr )
	{
		printf("can't judge! [%ld]\n", (uintptr_t)alloc_ptr);
	}
	else
	{
		printf("aligned success! [%ld, %ld]\n", (uintptr_t)alloc_ptr, (uintptr_t)aligned_ptr);
	}

	free(alloc_ptr);
}