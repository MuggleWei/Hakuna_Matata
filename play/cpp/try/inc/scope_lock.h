#ifndef __SCOPE_LOCK_H__
#define __SCOPE_LOCK_H__

#if WIN32
#include <windows.h>
class Mutex
{
public:
	inline Mutex()
	{
		mutex_ = ::CreateMutex(NULL, FALSE, NULL);
	}
	inline ~Mutex()
	{
		::CloseHandle(mutex_);
	}
	inline void Lock()
	{
		WaitForSingleObject(mutex_, INFINITE);
	}
	inline void Unlock()
	{
		::ReleaseMutex(mutex_);
	}

private:
	Mutex(const Mutex& m);
	Mutex& operator=(const Mutex &m);

private:
	HANDLE mutex_;
};
#else
#include <pthread.h>
class Mutex 
{
public:
	inline Mutex() 
	{
		::pthread_mutex_init(&mutex_, NULL);
	}
	inline ~Mutex()
	{
		::pthread_mutex_destroy(&mutex_);
	}
	inline void Lock() 
	{
		::pthread_mutex_lock(&mutex_);
	}
	inline void Unlock() 
	{
		::pthread_mutex_unlock(&mutex_);
	}	

private:
	Mutex(const Mutex& m);
	Mutex& operator=(const Mutex &m);

private:
	pthread_mutex_t mutex_;
};
#endif

/**
*	definition of ScopedLock;
**/
class ScopedLock {
private:
	Mutex& mutex_;

public:
	inline ScopedLock(Mutex& mutex)
		: mutex_(mutex)
	{
		mutex_.Lock();
	}

	inline ~ScopedLock() 
	{
		mutex_.Unlock();
	}
};

#endif