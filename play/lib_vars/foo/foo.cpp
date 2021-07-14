#include "foo.h"
#include <stdio.h>

Foo g_foo(VAR_TYPE_GLOBAL);

Foo Foo::s_class_static_var(VAR_TYPE_CLASS_STATIC);

const char* get_var_type_str(eVarType var_type)
{
	switch (var_type)
	{
		case eVarType::VAR_TYPE_CLASS_STATIC:
		{
			return "class_static";
		}break;
		case eVarType::VAR_TYPE_GLOBAL:
		{
			return "global";
		}break;
		case eVarType::VAR_TYPE_FUNC_STATIC:
		{
			return "func_static";
		}break;
		default:
		{
			return "unknown";
		}break;
	}
}

Foo::Foo(eVarType var_type_in)
	: var_type(var_type_in)
	, num(0)
{
	printf("%s - type=%s, this=0x%llx\n",
		__FUNCTION__, get_var_type_str(this->var_type),
		(unsigned long long)this);
}
Foo::~Foo()
{
	printf("%s - type=%s, this=0x%llx\n",
		__FUNCTION__, get_var_type_str(this->var_type),
		(unsigned long long)this);
}

Foo* Foo::InstanceClassStatic()
{
	return &s_class_static_var;
}
Foo* Foo::InstanceExternGlobal()
{
	return &g_foo;
}
Foo* Foo::InstanceFuncStatic()
{
	static Foo s_foo(VAR_TYPE_FUNC_STATIC);
	return &s_foo;
}
