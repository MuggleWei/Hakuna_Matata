#ifndef FOO_MATH_H_
#define FOO_MATH_H_

#include "foo/foo_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

FOO_EXPORT 
int foo_math_add(int a, int b);

FOO_EXPORT 
int foo_math_sub(int a, int b);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !FOO_UTILS_H_
