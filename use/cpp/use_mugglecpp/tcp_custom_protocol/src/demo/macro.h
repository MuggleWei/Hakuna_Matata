#ifndef MUGGLE_DEMO_MACRO_H_
#define MUGGLE_DEMO_MACRO_H_

#include "muggle/c/base/macro.h"

#if MUGGLE_PLATFORM_WINDOWS && defined(muggle_demo_USE_DLL)
	#ifdef muggle_demo_EXPORTS
		#define MUGGLE_DEMO_EXPORT __declspec(dllexport)
	#else
		#define MUGGLE_DEMO_EXPORT __declspec(dllimport)
	#endif
#else
	#define MUGGLE_DEMO_EXPORT
#endif


#define NS_MUGGLE_DEMO_BEGIN namespace muggle { namespace demo {
#define NS_MUGGLE_DEMO_END   }}
#define USING_NS_MUGGLE_DEMO using namespace muggle::demo

#endif // !MUGGLE_DEMO_MACRO_H_
