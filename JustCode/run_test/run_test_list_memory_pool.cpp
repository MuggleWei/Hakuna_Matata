#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include "list_memory_pool.h"

enum 
{
    block_size = 16,
};
struct Node
{
    unsigned char data[block_size];
};
typedef ListMemoryPool<block_size>::ListMemoryBlock ListMemoryBlock;

ListMemoryPool<block_size> k_memory_pool(4, true);

void ComparePerformance();
void SimpleUse();
void RandomUse();

int main(int argc, char* argv[])
{
    printf("performance compare: \n");
    ComparePerformance();
    fgetc(stdin);

    printf("simple use: \n");
    SimpleUse();
    fgetc(stdin);

    printf("randomly use: \n");
    RandomUse();
    fgetc(stdin);

    return 0;
}

void PrintOperate(unsigned int block_id, bool b_alloc)
{
#ifdef _DEBUG
    if (b_alloc)
    {
        if (block_id != -1)
        {
            printf("alloc operator: allocate memory id(%d)\n", block_id);
        }
        else
        {
            printf("alloc operator: can't allocate new block, memory is lack");
        }
    }
    else
    {
        printf("free operator: free memory id(%d)\n", block_id);
    }
#endif
}

void ComparePerformance()
{
    int num = 10240000;
    ListMemoryPool<block_size> memory_pool(num / 1024, true);
    clock_t start_clock, delta_clock;
    char** ptr_buf = (char**)malloc(sizeof(unsigned char*) * num);

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
    printf("memory pool alloc(need realloc): %ld\n", delta_clock);

    start_clock = clock();
    for (int i=0; i<num; ++i)
    {
        memory_pool.Free(memory_pool.working_list_head_.ptr_next);
    }
    delta_clock = clock() - start_clock;
    printf("memory pool free: %ld\n\n", delta_clock);

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
void SimpleUse()
{
    ListMemoryPool<block_size> memory_pool(1, true);
    unsigned int index_buf[8];
    int i = 0;

    for (i=0; i<8; ++i)
    {
        index_buf[i] = memory_pool.Alloc();
        Node* ptr_node = (Node*)memory_pool.GetData(index_buf[i]);
        // do something with ptr_node
        memory_pool.Print();
    }
    
    for (i=4; i>=0; --i)
    {
        memory_pool.Free(index_buf[i]);
        memory_pool.Print();
    }
    for (i=5; i<8; ++i)
    {
        memory_pool.Free(index_buf[i]);
        memory_pool.Print();
    }
}

void Update()
{
    unsigned int rand_num = rand() % 100;
    unsigned int block_id = -1;

    // allocate or free memory block randomly
    if (rand_num > 30)
    {
        block_id = k_memory_pool.Alloc();
        if (block_id != -1)
        {
            PrintOperate(block_id, true);
        }
        k_memory_pool.Print();
    }
    else
    {
        ListMemoryBlock* ptr_block = &k_memory_pool.working_list_head_;
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

            PrintOperate(ptr_block->id, false);
            k_memory_pool.Free(ptr_block);
            k_memory_pool.Print();
        }

    }
}
void RandomUse()
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