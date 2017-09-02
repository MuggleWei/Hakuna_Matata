#include "object.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"

static const char *s_names[] = {
	"foo", "bar"
};

typedef const char* (*func_get_name)(struct baseObj* obj);
typedef void (*func_print_object)(struct baseObj* obj);
typedef void* (*func_encode_object)(struct baseObj *obj, int *len);
typedef int (*func_decode_object)(void *buf, int len, struct baseObj *obj);

struct objFunc
{
	func_get_name fn_getname;
	func_print_object fn_print;
	func_encode_object fn_encode;
	func_decode_object fn_decode;
};

// foo
const char* fooGetName(struct baseObj* obj)
{
	return s_names[OBJ_TYPE_FOO];
}
void fooPrint(struct baseObj* obj)
{
	struct Foo *foo = (struct Foo*)obj;
	fprintf(stdout, "%s: %d, %s\n", fooGetName(obj), foo->foo_id, foo->buf);
}
void* fooEncode(struct baseObj *obj, int *len)
{
	struct Foo *foo = (struct Foo*)obj;

	size_t foo_buf_len = strnlen(foo->buf, sizeof(foo->buf));
	*len = (int)(sizeof(struct streamHead) + sizeof(int32_t) + foo_buf_len);

	char *buf = (char*)malloc(*len);
	if (buf == NULL)
	{
		return NULL;
	}
	char *p = buf + sizeof(struct streamHead);

	int32_t *pi = (int32_t*)p;
	*pi = ((struct Foo*)obj)->foo_id;
	*pi = util_hton_32(*pi);
	p += sizeof(int32_t);

	memcpy(p, foo->buf, foo_buf_len);

	return buf;
}
int fooDecode(void *buf, int len, struct baseObj *obj)
{
	struct Foo *foo = (struct Foo*)obj;

	const char *p = buf;
	p += sizeof(struct streamHead);

	foo->foo_id = util_ntoh_32(*(int32_t*)p);
	p += sizeof(int32_t);

	int buf_len = len - sizeof(struct streamHead) - sizeof(int32_t);
	if (buf_len >= sizeof(foo->buf))
	{
		return -1;
	}

	if (buf_len == 0)
	{
		foo->buf[0] = '\0';
	}
	else
	{
		memcpy(foo->buf, p, buf_len);
		foo->buf[buf_len] = '\0';
	}

	return 0;
}

// bar
const char* barGetName(struct baseObj* obj)
{
	return s_names[OBJ_TYPE_BAR];
}
void barPrint(struct baseObj* obj)
{
	struct Bar *bar = (struct Bar*)obj;
	fprintf(stdout, "%s: %d, %s\n", barGetName(obj), bar->bar_id, bar->buf);
}
void* barEncode(struct baseObj *obj, int *len)
{
	struct Bar *bar = (struct Bar*)obj;

	size_t bar_buf_len = strnlen(bar->buf, sizeof(bar->buf));
	*len = (int)(sizeof(struct streamHead) + bar_buf_len + sizeof(uint32_t));

	char *buf = (char*)malloc(*len);
	if (buf == NULL)
	{
		return NULL;
	}
	char *p = buf + sizeof(struct streamHead);

	memcpy(p, bar->buf, bar_buf_len);
	p += bar_buf_len;
	
	uint32_t *pu = (uint32_t*)p;
	*pu = bar->bar_id;
	*pu = util_hton_32(*pu);

	return buf;
}
int barDecode(void *buf, int len, struct baseObj *obj)
{
	struct Bar *bar = (struct Bar*)obj;

	const char *p = buf;
	p += sizeof(struct streamHead);

	int buf_len = len - sizeof(struct streamHead) - sizeof(bar->bar_id);
	if (buf_len >= sizeof(bar->buf))
	{
		return -1;
	}

	if (buf_len == 0)
	{
		bar->buf[0] = '\0';
	}
	else
	{
		memcpy(bar->buf, p, buf_len);
		bar->buf[buf_len] = '\0';
		p += buf_len;
	}
	
	bar->bar_id = util_ntoh_32(*(uint32_t*)p);

	return 0;
}

// callbacks
struct objFunc obj_fn[OBJ_TYPE_MAX] = {
	{ fooGetName, fooPrint, fooEncode, fooDecode },
	{ barGetName, barPrint, barEncode, barDecode }
};

const char* getName(struct baseObj *obj)
{
	if (obj->type >= 0 && obj->type < OBJ_TYPE_MAX)
	{
		return obj_fn[obj->type].fn_getname(obj);
	}

	return NULL;
}
void printObj(struct baseObj *obj)
{
	if (obj->type >= 0 && obj->type < OBJ_TYPE_MAX)
	{
		obj_fn[obj->type].fn_print(obj);
	}
}
void* encodeMsg(struct baseObj *obj, int *len)
{
	if (obj->type >= 0 && obj->type < OBJ_TYPE_MAX)
	{
		struct streamHead *head = (struct streamHead*)obj_fn[obj->type].fn_encode(obj, len);
		if (head)
		{
			head->size = util_hton_32((int32_t)*len);
			head->type = util_hton_32(obj->type);
			struct timeval tv;
			if (evutil_gettimeofday(&tv, NULL) == 0)
			{
				head->time_sec = util_hton_64((int64_t)tv.tv_sec);
				head->time_usec = util_hton_64((int64_t)tv.tv_usec);
			}

			return (void*)head;
		}
	}

	return NULL;
}
struct baseObj* decodeMsg(void *buf, int len)
{
	if (len < sizeof(struct streamHead))
	{
		return NULL;
	}

	const char *p = buf;
	int32_t size = util_ntoh_32(*(const int32_t*)p);
	p += sizeof(int32_t);

	int32_t type = util_ntoh_32(*(const int32_t*)p);
	if (type < 0 || type > OBJ_TYPE_MAX)
	{
		return NULL;
	}

	struct baseObj *obj = (struct baseObj*)malloc(size);
	if (obj == NULL)
	{
		return NULL;
	}

	obj->type = type;
	if (obj_fn[obj->type].fn_decode(buf, size, obj) != 0)
	{
		free(obj);
		return NULL;
	}

	// print encode time
	p += sizeof(int32_t);
	int64_t sec = util_ntoh_64(*(const int64_t*)p);
	p += sizeof(int64_t);
	int64_t usec = util_ntoh_64(*(const int64_t*)p);
	fprintf(stdout, "encode time: %ld.%06ld\n", (long)sec, (long)usec);

	return obj;
}
