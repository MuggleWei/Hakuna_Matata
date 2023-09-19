#include <iostream>
#include "adapter.h"

int main(int argc, char *argv[])
{
	Target *p_target = new Adapter();

	if (p_target != NULL)
	{
		p_target->Create();
		p_target->Bind();

		delete p_target;
		p_target = NULL;
	}

	return 0;
}