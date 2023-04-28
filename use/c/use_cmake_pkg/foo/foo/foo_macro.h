#ifndef FOO_MACRO_H_
#define FOO_MACRO_H_

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

#endif // !FOO_MACRO_H_
