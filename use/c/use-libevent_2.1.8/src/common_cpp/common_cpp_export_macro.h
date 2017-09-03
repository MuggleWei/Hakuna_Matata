#ifndef COMMON_CPP_H_
#define COMMON_CPP_H_

#if WIN32 && defined(common_cpp_USE_DLL)
#ifdef common_cpp_EXPORTS
#define common_cpp_EXPORT __declspec(dllexport)
#else
#define common_cpp_EXPORT __declspec(dllimport)
#endif
#else
#define common_cpp_EXPORT
#endif

#endif