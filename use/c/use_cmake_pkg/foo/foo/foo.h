#ifndef FOO_H_
#define FOO_H_

#include "foo/foo_config.h"

#if defined(_WIN32) && FOO_USE_DLL
	#if defined(FOO_EXPORTS)
		#define FOO_EXPORT __declspec(dllexport)
	#else
		#define FOO_EXPORT __declspec(dllimport)
	#endif
#else
	#define FOO_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

FOO_EXPORT 
int foo_add(int a, int b);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !FOO_H_
