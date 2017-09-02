#ifndef COMMON_H_
#define COMMON_H_

#if WIN32 && defined(common_USE_DLL)
	#ifdef common_EXPORTS
		#define common_EXPORT __declspec(dllexport)
	#else
		#define common_EXPORT __declspec(dllimport)
	#endif
#else
	#define common_EXPORT
#endif

#endif