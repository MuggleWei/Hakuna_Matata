#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include "memory_pool.h"

enum 
{
	block_size = 16,
};
struct Node
{
	unsigned char data[block_size];
};
MemoryPool<block_size> k_memory_pool;
typedef MemoryPool<block_size>::MemoryBlock MemoryBlock;

void UseMemoryPool();
void Update();
void PrintOperate(MemoryBlock* ptr_block, bool b_alloc);

void ComparePerformance();

int main(int argc, char* argv[])
{
	ComparePerformance();
	fgetc(stdin);

	UseMemoryPool();
	fgetc(stdin);

	return 0;
}

void UseMemoryPool()
{
	clock_t cur_clock = 0, old_clock = 0;
	srand((unsigned int)time(NULL));

	k_memory_pool.Print();

	while (1)
	{
		// time interval
		cur_clock = clock();
		if ( cur_clock - old_clock < 1 )
		{
			continue;
		}
		old_clock = cur_clock;

		Update();
	}
}
void Update()
{
	unsigned int rand_num = rand() % 100;
	MemoryBlock* ptr_block = NULL;

	// allocate or free memory block randomly
	if (rand_num > 30)
	{
		ptr_block = k_memory_pool.Alloc();
		if (ptr_block != NULL)
		{
			PrintOperate(ptr_block, true);
		}
		k_memory_pool.Print();
	}
	else
	{
		ptr_block = &k_memory_pool.working_list_head_;
		if (ptr_block->ptr_next == &k_memory_pool.working_list_tail_)
		{
			return;
		}
		else
		{
			rand_num = rand() % 50;
			ptr_block = k_memory_pool.working_list_head_.ptr_next;
			while (rand_num > 0)
			{
				rand_num--;
				ptr_block = ptr_block->ptr_next;
				if (ptr_block == &k_memory_pool.working_list_tail_)
				{
					ptr_block = k_memory_pool.working_list_head_.ptr_next;
				}
			}

			PrintOperate(ptr_block, false);
			k_memory_pool.Free(ptr_block);
			k_memory_pool.Print();
		}

	}
}
void PrintOperate(MemoryBlock* ptr_block, bool b_alloc)
{
#ifdef _DEBUG
	if (b_alloc)
	{
		if (ptr_block != NULL)
		{
			printf("alloc operator: allocate memory id(%d)\n", ptr_block->id);
		}
		else
		{
			printf("alloc operator: can't allocate new block, memory is lack");
		}
	}
	else
	{
		printf("free operator: free memory id(%d)\n", ptr_block->id);
	}
#endif
}

void ComparePerformance()
{
	int num = 1024000;
	MemoryPool<block_size> memory_pool(num, false);
	clock_t start_clock, delta_clock;
	char** ptr_buf = (char**)malloc(sizeof(unsigned char*) * num);

	printf("performance compare: \n");

	start_clock = clock();
	for (int i=0; i<num; ++i)
	{
		ptr_buf[i] = (char*)malloc(block_size);
	}
	delta_clock = clock() - start_clock;
	printf("malloc: %ld\n", delta_clock);

	start_clock = clock();
	for (int i=0; i<num; ++i)
	{
		free(ptr_buf[i]);
	}
	delta_clock = clock() - start_clock;
	printf("free: %ld\n\n", delta_clock);

	start_clock = clock();
	for (int i=0; i<num; ++i)
	{
		ptr_buf[i] = new char[block_size];
	}
	delta_clock = clock() - start_clock;
	printf("new: %ld\n", delta_clock);

	start_clock = clock();
	for (int i=0; i<num; ++i)
	{
		delete[](ptr_buf[i]);
	}
	delta_clock = clock() - start_clock;
	printf("delete: %ld\n\n", delta_clock);

	start_clock = clock();
	for (int i=0; i<num; ++i)
	{
		memory_pool.Alloc();
	}
	delta_clock = clock() - start_clock;
	printf("memory pool alloc: %ld\n", delta_clock);

	start_clock = clock();
	for (int i=0; i<num; ++i)
	{
		memory_pool.Free(memory_pool.working_list_head_.ptr_next);
	}
	delta_clock = clock() - start_clock;
	printf("memory pool free: %ld\n\n", delta_clock);

	free(ptr_buf);
}