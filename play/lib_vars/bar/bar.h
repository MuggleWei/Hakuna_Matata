#ifndef BAR_H_
#define BAR_H_

#if WIN32 && defined(bar_USE_DLL)
	#ifdef bar_EXPORTS
		#define BAR_EXPORT __declspec(dllexport)
	#else
		#define BAR_EXPORT __declspec(dllimport)
	#endif
#else
	#define BAR_EXPORT
#endif

BAR_EXPORT
void Bar();

#endif /* ifndef BAR_H_ */
