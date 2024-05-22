#ifndef PFPA_MACRO_H_
#define PFPA_MACRO_H_

#include "muggle/c/base/macro.h"
#include "pfpa/pfpa_config.h"
#include <stdbool.h>
#include <stdint.h>

// lib and dll
#if MUGGLE_PLATFORM_WINDOWS && defined(PFPA_USE_DLL)
	#ifdef PFPA_EXPORTS
		#define PFPA_EXPORT __declspec(dllexport)
	#else
		#define PFPA_EXPORT __declspec(dllimport)
	#endif
#else
	#define PFPA_EXPORT
#endif

#endif // !PFPA_MACRO_H_
