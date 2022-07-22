#include <stdio.h>
#include "foo/foo.h"
#include "bar/bar.h"

int main()
{
	Foo *p_class_static = Foo::InstanceClassStatic();
	p_class_static->num += 1000;
	printf("In Hello, Foo::InstanceClassStatic=0x%llx, num=%d\n",
		(unsigned long long)p_class_static, p_class_static->num);

	Foo *p_global = Foo::InstanceExternGlobal();
	p_global->num += 1000;
	printf("In Hello, Foo::InstanceExternGlobal=0x%llx, num=%d\n",
		(unsigned long long)p_global, p_global->num);

	Foo *p_func_static = Foo::InstanceFuncStatic();
	p_func_static->num += 1000;
	printf("In Hello, Foo::InstanceFuncStatic=0x%llx, num=%d\n",
		(unsigned long long)p_func_static, p_func_static->num);

	Bar();

	return 0;
}
