#ifndef OBJECT_H_
#define OBJECT_H_

#include <stdint.h>
#include <event2/event.h>
#include "common/common_export_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

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

common_EXPORT const char* getName(struct baseObj *obj);
common_EXPORT void printObj(struct baseObj *obj);
common_EXPORT void* encodeMsg(struct baseObj *obj, int *len);
common_EXPORT struct baseObj* decodeMsg(void *buf, int len);

#ifdef __cplusplus
}
#endif


#endif