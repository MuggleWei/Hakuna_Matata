#ifndef __MEMORY_CHECK_H__
#define __MEMORY_CHECK_H__

#ifdef _WIN32

#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>

class MemoryCheck
{
public:
    void Start()
    {
        _CrtMemCheckpoint(&s1_);
    }
    void End()
    {
        _CrtMemCheckpoint(&s2_);
    }
    void Statistics()
    {
        _CrtMemState s;
        if (_CrtMemDifference(&s, &s1_, &s2_))
        {
            _CrtMemDumpStatistics(&s);
            _CrtDumpMemoryLeaks();
        }
    }

private:
    _CrtMemState s1_, s2_;
};

#else

#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

class MemoryCheck
{
public:
    MemoryCheck()
    {
        s_in_trace_ = false;
    }

    void Start()
    {
        if (s_in_trace_)
        {
            return;
        }
        setenv("MALLOC_TRACE", "memory_leak_file", 1);
        mtrace();
    }
    void End()
    {
        muntrace();
        s_in_trace_ = false;
    }
    void Statistics()
    {
        FILE *fp = fopen("memory_leak_file", "r");
        if (fp == NULL)
        {
            return;
        }

        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char* buf = (char*)malloc(size);
        fread(buf, 1, size+1, fp);
        printf("%s\n", buf);
        free(buf);

        fclose(fp);
    }

private:
    static bool s_in_trace_;
};

#endif

#endif