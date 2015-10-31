#ifndef __MEMORY_CHECK_H__
#define __MEMORY_CHECK_H__

#ifdef _WIN32

#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>

class MemoryCheck
{
public:
    MemoryCheck()
    {
        _CrtMemCheckpoint(&s1_);
        _CrtMemCheckpoint(&s2_);
    }

    void Start()
    {
        if (s_in_trace_)
        {
            printf("warning: Nested call will error in linux!\n");
        }
        _CrtMemCheckpoint(&s1_);
        s_in_trace_ = true;
    }
    void End()
    {
        _CrtMemCheckpoint(&s2_);
        s_in_trace_ = false;
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
    static bool s_in_trace_;
};

bool MemoryCheck::s_in_trace_ = false;

#else

#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

class MemoryCheck
{
public:
    MemoryCheck()
    {}

    void Start()
    {
        if (s_in_trace_)
        {
            printf("error: Nested call is not allowed!\n");
            return;
        }
        setenv("MALLOC_TRACE", "memory_leak_file", 1);
        mtrace();
        s_in_trace_ = true;
    }
    void End()
    {
        muntrace();
        s_in_trace_ = false;
    }
    void Statistics()
    {
        system("cat memory_leak_file");
        system("mtrace memory_leak_file");
    }

private:
    static bool s_in_trace_;
};

bool MemoryCheck::s_in_trace_ = false;

#endif

#endif