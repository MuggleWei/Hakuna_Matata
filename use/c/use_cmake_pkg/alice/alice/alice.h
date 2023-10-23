#ifndef ALICE_H_
#define ALICE_H_

#include "alice/alice_config.h"

#if defined(_WIN32) && ALICE_USE_DLL
	#if defined(ALICE_EXPORTS)
		#define ALICE_EXPORT __declspec(dllexport)
	#else
		#define ALICE_EXPORT __declspec(dllimport)
	#endif
#else
	#define ALICE_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

ALICE_EXPORT
int alice_add(int a, int b);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !ALICE_H_
