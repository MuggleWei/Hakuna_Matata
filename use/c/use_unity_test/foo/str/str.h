#ifndef FOO_STR_H_
#define FOO_STR_H_

#include "foo/foo_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

FOO_EXPORT
char* foo_str_join(const char *s1, const char *s2);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !FOO_STR_H_
