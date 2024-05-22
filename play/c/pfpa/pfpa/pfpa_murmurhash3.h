// ----------------------------------------------------------------------------
//                           MurmurHash
//
// From: https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
// ----------------------------------------------------------------------------

#ifndef PFPA_MURMURHASH3_H_
#define PFPA_MURMURHASH3_H_

#include "pfpa_macro.h"

EXTERN_C_BEGIN

PFPA_EXPORT
void pfpa_MurmurHash3_x86_32(const void *key, int len, uint32_t seed,
							 void *out);

EXTERN_C_END

#endif // !PFPA_MURMURHASH3_H_
