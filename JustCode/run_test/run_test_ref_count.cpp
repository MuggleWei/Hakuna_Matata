#include <string>
#include "ref_count.h"

static RefCount<std::string> process(RefCount<std::string> ref_count)
{
    auto tmp = ref_count;
    return tmp;
}

int main(int argc, char* argv[])
{
    {
        RefCount<std::string> r1(new std::string("xxx"));
        RefCount<std::string> r2(r1);
        RefCount<std::string> r3;
        r3 = r2;
        r3 = r1;
        r1 = RefCount<std::string>();
        r1 = RefCount<std::string>(new std::string("yyy"));
        r3 = r1;
        r2 = process(r3);
    }

    return 0;
}