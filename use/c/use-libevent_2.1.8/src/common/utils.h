#ifndef UTILS_H_
#define UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "common_export_macro.h"

// endian
common_EXPORT extern int g_detect_endian;
#define IS_LITTLE_ENDIAN (*(char*)&g_detect_endian)

#define util_ex_32(v) \
((((v)&0xff) << 24) | \
(((v) & 0xff00) << 8) | \
(((v) & 0xff0000) >> 8) | \
(((v) & 0xff000000) >> 24))

#define util_ex_64(v) \
((((v)&0xff) << 56) | \
(((v) & 0xff00) << 40) | \
(((v) & 0xff0000) << 24) | \
(((v) & 0xff000000) << 8) | \
(((v) & 0xff00000000) >> 8) | \
(((v) & 0xff0000000000) >> 24) | \
(((v) & 0xff000000000000) >> 40) | \
(((v) & 0xff00000000000000) >> 56))

#define util_hton_32(v) (IS_LITTLE_ENDIAN ? util_ex_32(v) : v)
#define util_hton_64(v) (IS_LITTLE_ENDIAN ? util_ex_64(v) : v)
#define util_ntoh_32(v) util_hton_32(v)
#define util_ntoh_64(v) util_hton_64(v)

common_EXPORT const char* get_sockaddr_port(const struct sockaddr *sa, char *out, size_t outlen);

#ifdef __cplusplus
}
#endif

#endif