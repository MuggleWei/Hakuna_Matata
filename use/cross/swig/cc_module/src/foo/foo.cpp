#include "foo.h"
#include <stdio.h>
#include <string.h>
#include <thread>

FooSpi::~FooSpi()
{}

void FooSpi::syncCallback(int /*x*/)
{
	// sub-class override
}
void FooSpi::asyncCallback(int /*x*/)
{
	// sub-class override
}
void FooSpi::syncCallbackVar(Bar /*bar*/)
{
	// sub-class override
}
void FooSpi::syncCallbackRef(Bar & /*bar*/)
{
	// sub-class override
}
void FooSpi::syncCallbackPtr(Bar * /*bar*/)
{
	// sub-class override
}
void FooSpi::onReadFromFile(const char * /*filepath*/, foo_msg_t * /*msg*/)
{
	// sub-class override
}

void FooApi::setSpi(FooSpi *spi)
{
	spi_ = spi;
}

bool FooApi::syncCall(int x)
{
	spi_->syncCallback(x);
	return true;
}

bool FooApi::asyncCall(int x)
{
	std::thread th([&, x]{
		spi_->asyncCallback(x);
	});
	th.join();

	return true;
}

bool FooApi::syncCallVar(Bar bar)
{
	spi_->syncCallbackVar(bar);
	return true;
}

bool FooApi::syncCallRef(Bar &bar)
{
	spi_->syncCallbackRef(bar);
	return true;
}

bool FooApi::syncCallPtr(Bar *bar)
{
	spi_->syncCallbackPtr(bar);
	return true;
}

void FooApi::readFile(const char *filepath)
{
	foo_msg_t msg;
	memset(&msg, 0, sizeof(msg));

	FILE *fp = fopen(filepath, "rb");

	if (fp != NULL)
	{
		fread(msg.content, 1, sizeof(msg.content), fp);
	}

	spi_->onReadFromFile(filepath, &msg);

	if (fp != NULL)
	{
		fclose(fp);
	}
}
