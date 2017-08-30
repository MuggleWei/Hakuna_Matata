#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdint.h>
#include <event2/event.h>

#ifdef __cplusplus
extern "C" {
#endif

// endian
extern int g_detect_endian;
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


enum
{
	OBJ_TYPE_FOO = 0,
	OBJ_TYPE_BAR,
	OBJ_TYPE_MAX,
};

struct baseObj
{
	int32_t type;
};

struct Foo
{
	struct baseObj obj;
	int32_t foo_id;
	char buf[32];
};
struct Bar
{
	struct baseObj obj;
	char buf[32];
	uint32_t bar_id;
};

struct streamHead
{
	int32_t size;
	int32_t type;
	int64_t time_sec;
	int64_t time_usec;
};

const char* getName(struct baseObj *obj);
void printObj(struct baseObj *obj);
void* encodeMsg(struct baseObj *obj, int *len);
struct baseObj* decodeMsg(void *buf, int len);

#ifdef __cplusplus
}
#endif


#endif