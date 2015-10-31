#include <malloc.h>
#include "memory_check.h"

int main()
{
    MemoryCheck mc_leak, mc;

    mc.Start();

    mc_leak.Start();

    int* p = (int*)malloc(sizeof(int) * 10);

    mc_leak.End();
    mc_leak.Statistics();

    free(p);

    mc.End();
    mc.Statistics();

    return 0;
}