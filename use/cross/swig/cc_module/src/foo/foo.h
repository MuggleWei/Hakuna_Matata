#ifndef CC_MODULE_FOO_H_
#define CC_MODULE_FOO_H_

#include "foo_macro.h"
#include "foo_struct.h"

struct foo_msg_t
{
	char content[128];
};

class FooSpi
{
public:
	virtual ~FooSpi();
	virtual void syncCallback(int x);
	virtual void asyncCallback(int x);
	virtual void syncCallbackVar(Bar bar);
	virtual void syncCallbackRef(Bar &bar);
	virtual void syncCallbackPtr(Bar *bar);
	virtual void onReadFromFile(const char *filepath, foo_msg_t *msg);
};

class FooApi
{
public:
	CC_MODULE_FOO_EXPORT
	void setSpi(FooSpi *spi);

	CC_MODULE_FOO_EXPORT
	bool syncCall(int x);

	CC_MODULE_FOO_EXPORT
	bool asyncCall(int x);

	CC_MODULE_FOO_EXPORT
	bool syncCallVar(Bar bar);

	CC_MODULE_FOO_EXPORT
	bool syncCallRef(Bar &bar);

	CC_MODULE_FOO_EXPORT
	bool syncCallPtr(Bar *bar);

	CC_MODULE_FOO_EXPORT
	void readFile(const char *filepath);

private:
	FooSpi *spi_;
};

#endif
