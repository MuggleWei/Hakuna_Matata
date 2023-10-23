#ifndef DAVE_H_
#define DAVE_H_

#include "dave/dave_config.h"

#if defined(_WIN32) && DAVE_USE_DLL
	#if defined(DAVE_EXPORTS)
		#define DAVE_EXPORT __declspec(dllexport)
	#else
		#define DAVE_EXPORT __declspec(dllimport)
	#endif
#else
	#define DAVE_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

DAVE_EXPORT
int dave_add(int a, int b);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !DAVE_H_
