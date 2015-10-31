#ifndef __DELEGATE_H__
#define __DELEGATE_H__

template<typename ReturnType>
class StaticDelegateVoid
{
    typedef ReturnType (*FunctionPointer)();

public:
    StaticDelegateVoid()
    {
        m_ptrFunc = nullptr;
    }
    void Bind(FunctionPointer ptrFunc)
    {
        m_ptrFunc = ptrFunc;
    }
    void Invoke()
    {
        if (m_ptrFunc)
        {
            (m_ptrFunc)();
        }
    }

private:
    FunctionPointer m_ptrFunc;
};

template<typename ReturnType, typename Arg1>
class StaticDelegateOne
{
    typedef ReturnType(*FunctionPointer)(Arg1);

public:
    StaticDelegateOne()
    {
        m_ptrFunc = nullptr;
    }
    void Bind(FunctionPointer ptrFunc)
    {
        m_ptrFunc = ptrFunc;
    }
    void Invoke(Arg1 arg1)
    {
        if (m_ptrFunc)
        {
            (m_ptrFunc)(arg1);
        }
    }

private:
    FunctionPointer m_ptrFunc;
};


template<typename TClass, typename ReturnType>
class DelegateVoid
{
    typedef ReturnType (TClass::*FunctionPointer)();

public:
    DelegateVoid()
    {
        m_instance = nullptr;
        m_ptrFunc = nullptr;
    }
    void Bind(TClass* instance, FunctionPointer ptrFunc)
    {
        m_instance = instance;
        m_ptrFunc = ptrFunc;
    }
    void Invoke()
    {
        if (m_instance && m_ptrFunc)
        {
            (m_instance->*m_ptrFunc)();
        }
    }

private:
    TClass* m_instance;
    FunctionPointer m_ptrFunc;
};

#endif