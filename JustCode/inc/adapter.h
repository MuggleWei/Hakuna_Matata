#ifndef __ADAPTER_H__
#define __ADAPTER_H__

class Adaptee
{
public:
	void WrappedCreateFunc();
	void WrappedBindFunc();
};

class Target
{
public:
	virtual ~Target() {}

	virtual void Create() = 0;
	virtual void Bind() = 0;
};

class Adapter : public Target
{
public:
	Adapter();
	virtual ~Adapter();

	virtual void Create() override;
	virtual void Bind() override;

private:
	Adaptee *adaptee_;
};

#endif