#include <stdio.h>
#include "timer.h"

int main()
{
    Timer t;

    t.Start();
    int j;
    for (int i = 0; i < 10240; ++i)
    {
        j = i*5;
    }
    t.End();

    printf("%f\n", t.GetElapsedMilliseconds());

    return 0;
}