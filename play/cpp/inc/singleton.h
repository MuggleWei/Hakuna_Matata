#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <stdlib.h>
#include "scope_lock.h"

// version 1 - a simple singleton
/*
class Singleton
{
public:
	static Singleton& Instance()
	{
		static Singleton& singleton;
		return singleton;
	}

private:
	Singleton() {}
};
*/

// version 2 - consider reuse
/*
template<typename T>
class Singleton
{
public:
	static T& Instance()
	{
		static Singleton& singleton;
		return singleton;
	}

protected:
	Singleton() {}
	virtual ~Singleton() {}

private:
	Singleton(const Singleton& rhs) {}
	Singleton& operator=(const Singleton& rhs) {}
};
*/

// version 3 - consider lifetime manager and thread safe
/*
template<typename T>
class Singleton
{
public:
	static T& Instance()
	{
		if (singleton_ == NULL)
		{
			ScopedLock lock(mutex_);
			if (singleton_ == NULL)
			{
				singleton_ = new T();
				atexit(Destroy);
			}
		}

		return *singleton_;
	}

protected:
	Singleton() {}
	virtual ~Singleton() {}

private:
	Singleton(const Singleton& rhs) {}
	Singleton& operator=(const Singleton& rhs) {}

	static void Destroy()
	{
		if (singleton_ != NULL)
		{
			delete singleton_;
			singleton_ = NULL;
		}
	}

private:
	static T* volatile singleton_;
	static Mutex mutex_;
};

template<typename T>
T* volatile Singleton<T>::singleton_ = NULL;
template<typename T>
Mutex Singleton<T>::mutex_;
*/

// version 4 - consider constructor and use c++11 variadic templates
template<typename T>
class Singleton
{
public:
	template<typename... Args>
	static T& Instance(Args&&... args)
	{
		if (singleton_ == NULL)
		{
			ScopedLock lock(mutex_);
			if (singleton_ == NULL)
			{
				singleton_ = new T(std::forward<Args>(args)...);
				atexit(Destroy);
			}
		}

		return *singleton_;
	}

protected:
	Singleton() {}
	virtual ~Singleton() {}

private:
	Singleton(const Singleton& rhs) {}
	Singleton& operator=(const Singleton& rhs) {}

	static void Destroy()
	{
		if (singleton_ != NULL)
		{
			delete singleton_;
			singleton_ = NULL;
		}
	}

private:
	static T* volatile singleton_;
	static Mutex mutex_;
};

template<typename T>
T* volatile Singleton<T>::singleton_ = NULL;
template<typename T>
Mutex Singleton<T>::mutex_;

#endif