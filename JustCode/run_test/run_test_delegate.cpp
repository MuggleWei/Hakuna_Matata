#include <stdio.h>
#include <functional>
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
        id_ = s_id++;
    }
    virtual void VirtualMemberFuncVoid()
    {
        printf("call %d BaseObject VirtualMemberFuncVoid()\n", id_);
    }
    virtual void VritualMemberFuncOne(int i)
    {
        printf("call %d BaseObject VritualMemberFuncOne(%d)\n", id_, i);
    }
    void MemberFuncVoid()
    {
        printf("call %d BaseObject MemberFuncVoid()\n", id_);
    }

protected:
    int id_;
};

class DerivedObj : public BaseObject
{
public:
    virtual void VirtualMemberFuncVoid()
    {
        printf("call %d DerivedObj VirtualMemberFuncVoid()\n", id_);
    }
    virtual void VritualMemberFuncOne(int i)
    {
        printf("call %d DerivedObj VritualMemberFuncOne(%d)\n", id_, i);
    }
    void DerivedMemberFuncVoid()
    {
        printf("call %d DerivedObj DerivedMemberFuncVoid()\n", id_);
    }
};

int main(int argc, char* argv[])
{
    // function
    printf("function: \n");

    StaticDelegateVoid<void> static_delegate_void;
    static_delegate_void.Bind(FuncVoid);
    static_delegate_void.Invoke();

    StaticDelegateOne<void, int> static_delegate_one;
    static_delegate_one.Bind(FuncOne);
    static_delegate_one.Invoke(5);

    printf("\n");

    BaseObject base_obj;
    DerivedObj derived_obj;

    // member function
    printf("member function: \n");

    DelegateVoid<BaseObject, void> delegate_void;
    DelegateOne<BaseObject, void, int> delegate_one;

    delegate_void.Bind(&base_obj, &BaseObject::VirtualMemberFuncVoid);
    delegate_void.Invoke();
    delegate_one.Bind(&base_obj, &BaseObject::VritualMemberFuncOne);
    delegate_one.Invoke(5);
    delegate_void.Bind(&base_obj, &BaseObject::MemberFuncVoid);
    delegate_void.Invoke();

    delegate_void.Bind(&derived_obj, &BaseObject::VirtualMemberFuncVoid);
    delegate_void.Invoke();
    delegate_one.Bind(&derived_obj, &BaseObject::VritualMemberFuncOne);
    delegate_one.Invoke(5);
    typedef void (BaseObject::*MemberFuncVoidPtr)();
    delegate_void.Bind(&derived_obj, (MemberFuncVoidPtr)&DerivedObj::DerivedMemberFuncVoid);
    delegate_void.Invoke();

    printf("\n");

    // use c11
    printf("member function use c11\n");

    DelegateVoidC11<BaseObject, void> delegate_void_c11;
    DelegateOneC11<BaseObject, void, int> delegate_one_c11;

    delegate_void_c11.Bind(&base_obj, &BaseObject::VirtualMemberFuncVoid);
    delegate_void_c11.Invoke();
    delegate_one_c11.Bind(&base_obj, &BaseObject::VritualMemberFuncOne);
    delegate_one_c11.Invoke(5);
    delegate_void_c11.Bind(&base_obj, &BaseObject::MemberFuncVoid);
    delegate_void_c11.Invoke();

    delegate_void_c11.Bind(&derived_obj, &BaseObject::VirtualMemberFuncVoid);
    delegate_void_c11.Invoke();
    delegate_one_c11.Bind(&derived_obj, &BaseObject::VritualMemberFuncOne);
    delegate_one_c11.Invoke(5);
    typedef void (BaseObject::*MemberFuncVoidPtr)();
    delegate_void_c11.Bind(&derived_obj, (MemberFuncVoidPtr)&DerivedObj::DerivedMemberFuncVoid);
    delegate_void_c11.Invoke();

    printf("\n");

    return 0;
}