#include <malloc.h>
#include "memory_check.h"

int main()
{
#ifdef _WIN32
    MemoryCheck mc_leak, mc;

    mc.Start();

    mc_leak.Start();

    int* p = (int*)malloc(sizeof(int) * 10);

    mc_leak.End();
    mc_leak.Statistics();

    free(p);

    mc.End();
    mc.Statistics();
#else
    MemoryCheck mc;
    mc.Start();

    for (int i=0; i<10; ++i)
    {
        int* p = (int*)malloc(sizeof(int) * 10);
        // free(p);
    }
    

    mc.End();
    mc.Statistics();

    mc.Start();

    for (int i=0; i<10; ++i)
    {
        int* p = (int*)malloc(sizeof(int) * 10);
        free(p);
    }
    

    mc.End();
    mc.Statistics();

    
#endif

    return 0;
}