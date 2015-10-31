#include <stdio.h>
#include "Delegate.h"

void FuncVoid()
{
    printf("call FuncVoid()\n");
}
void FuncOne(int i)
{
    printf("call FuncOne(%d)\n", i);
}

class BaseObject
{
public:
    BaseObject()
    {
        static int s_id = 0;
        m_id = s_id++;
    }
    virtual void VirtualMemberFuncVoid()
    {
        printf("call %d BaseObject VirtualMemberFuncVoid()\n", m_id);
    }
    void MemberFuncVoid()
    {
        printf("call %d BaseObject MemberFuncVoid()\n", m_id);
    }

protected:
    int m_id;
};

class DerivedObj : public BaseObject
{
public:
    virtual void VirtualMemberFuncVoid()
    {
        printf("call %d DerivedObj VirtualMemberFuncVoid()\n", m_id);
    }
    void DerivedMemberFuncVoid()
    {
        printf("call %d DerivedObj DerivedMemberFuncVoid()\n", m_id);
    }
};

int main(int argc, char* argv[])
{
    StaticDelegateVoid<void> staticDelegateVoid;
    staticDelegateVoid.Bind(FuncVoid);
    staticDelegateVoid.Invoke();

    StaticDelegateOne<void, int> staticDelegateOne;
    staticDelegateOne.Bind(FuncOne);
    staticDelegateOne.Invoke(5);

    BaseObject baseObj;
    DelegateVoid<BaseObject, void> delegateVoid;
    delegateVoid.Bind(&baseObj, &BaseObject::MemberFuncVoid);
    delegateVoid.Invoke();

    delegateVoid.Bind(&baseObj, &BaseObject::VirtualMemberFuncVoid);
    delegateVoid.Invoke();

    DerivedObj derivedObj;
    delegateVoid.Bind(&derivedObj, &BaseObject::VirtualMemberFuncVoid);
    delegateVoid.Invoke();

    typedef void (BaseObject::*MemberFuncVoidPtr)();
    delegateVoid.Bind(&derivedObj, (MemberFuncVoidPtr)&DerivedObj::DerivedMemberFuncVoid);
    delegateVoid.Invoke();

    return 0;
}