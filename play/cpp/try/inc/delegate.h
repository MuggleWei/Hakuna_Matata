#ifndef __DELEGATE_H__
#define __DELEGATE_H__

#include <functional>

template<typename ReturnType>
class StaticDelegateVoid
{
    typedef ReturnType (*FunctionPointer)();

public:
    StaticDelegateVoid()
    {
        ptr_func_ = nullptr;
    }
    void Bind(FunctionPointer ptr_func)
    {
        ptr_func_ = ptr_func;
    }
    void Invoke()
    {
        if (ptr_func_)
        {
            (ptr_func_)();
        }
    }

private:
    FunctionPointer ptr_func_;
};

template<typename ReturnType, typename Arg1>
class StaticDelegateOne
{
    typedef ReturnType(*FunctionPointer)(Arg1);

public:
    StaticDelegateOne()
    {
        ptr_func_ = nullptr;
    }
    void Bind(FunctionPointer ptr_func)
    {
        ptr_func_ = ptr_func;
    }
    void Invoke(Arg1 arg1)
    {
        if (ptr_func_)
        {
            (ptr_func_)(arg1);
        }
    }

private:
    FunctionPointer ptr_func_;
};


template<typename TClass, typename ReturnType>
class DelegateVoid
{
    typedef ReturnType (TClass::*FunctionPointer)();

public:
    DelegateVoid()
    {
        instance_ = nullptr;
        ptr_func_ = nullptr;
    }
    void Bind(TClass* instance, FunctionPointer ptr_func)
    {
        instance_ = instance;
        ptr_func_ = ptr_func;
    }
    void Invoke()
    {
        if (instance_ && ptr_func_)
        {
            (instance_->*ptr_func_)();
        }
    }

private:
    TClass* instance_;
    FunctionPointer ptr_func_;
};

template<typename TClass, typename ReturnType, typename Arg1>
class DelegateOne
{
    typedef ReturnType (TClass::*FunctionPointer)(Arg1);

public:
    DelegateOne()
    {
        instance_ = nullptr;
        ptr_func_ = nullptr;
    }
    void Bind(TClass* instance, FunctionPointer ptr_func)
    {
        instance_ = instance;
        ptr_func_ = ptr_func;
    }
    void Invoke(Arg1 arg1)
    {
        if (instance_ && ptr_func_)
        {
            (instance_->*ptr_func_)(arg1);
        }
    }

private:
    TClass* instance_;
    FunctionPointer ptr_func_;
};

template<typename TClass, typename ReturnType>
class DelegateVoidC11
{
    typedef ReturnType (TClass::*FunctionPointer)();

public:
    DelegateVoidC11()
    {
        ptr_func_ = nullptr;
    }
    void Bind(TClass* instance, FunctionPointer ptr_func)
    {
        ptr_func_ = std::bind(ptr_func, instance);
    }
    void Invoke()
    {
        if (ptr_func_)
        {
            ptr_func_();
        }
    }

private:
    std::function<ReturnType()> ptr_func_;
};

template<typename TClass, typename ReturnType, typename Arg1>
class DelegateOneC11
{
    typedef ReturnType(TClass::*FunctionPointer)(Arg1);

public:
    DelegateOneC11()
    {
        ptr_func_ = nullptr;
    }
    void Bind(TClass* instance, FunctionPointer ptr_func)
    {
        ptr_func_ = std::bind(ptr_func, instance, std::placeholders::_1);
    }
    void Invoke(Arg1 arg1)
    {
        if (ptr_func_)
        {
            ptr_func_(arg1);
        }
    }

private:
    std::function<ReturnType(Arg1)> ptr_func_;
};

#endif