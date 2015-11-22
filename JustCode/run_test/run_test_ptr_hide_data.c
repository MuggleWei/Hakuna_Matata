#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ptr_hide_data.h"
#include "memory_aligned.h"

int main(int argc, char* argv[])
{
    void* ptr_alloc = NULL;
    uintptr_t ptr = 0;
    int data       = 2;
    int hide_data = 0;

    // uintptr_t ptr = (uintptr_t)aligned_malloc(4, 16);
    ptr = AlignedAlloc(4, 16, (void**)&ptr_alloc);

    printf("alloc pointer: %ld\n", ptr);
    printf("data: %d\n", data);

    PTR_HIDE_DATA_SET_DATA(ptr, data);
    printf("pointer hide data: %ld\n", ptr);

    hide_data = PTR_HIDE_DATA_GET_DATA(ptr);
    ptr = PTR_HIDE_DATA_GET_PTR(ptr);
    printf("pointer get data: %d\n", hide_data);
    printf("pointer get ptr: %ld\n", ptr);

    free(ptr_alloc);

    return 0;
}