#include "adapter.h"
#include <iostream>

void Adaptee::WrappedCreateFunc()
{
	std::cout << "Wrapped specific create function" << std::endl;
}
void Adaptee::WrappedBindFunc()
{
	std::cout << "Wrapped specific bind function" << std::endl;
}

Adapter::Adapter()
{
	adaptee_ = new Adaptee();
}
Adapter::~Adapter()
{
	if (adaptee_ != NULL)
	{
		delete adaptee_;
		adaptee_ = NULL;
	}
}
void Adapter::Create()
{
	if (adaptee_ != NULL)
	{
		adaptee_->WrappedCreateFunc();
	}
}
void Adapter::Bind()
{
	if (adaptee_ != NULL)
	{
		adaptee_->WrappedBindFunc();
	}
}