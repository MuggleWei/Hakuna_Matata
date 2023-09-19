#ifndef __ENUM_STRING_H__
#define __ENUM_STRING_H__

// common enum
enum Enum1
{
	E1_A,
	E1_B,
	E1_MAX,
};
extern const char* EnumString1[E1_MAX];

// enum macro
#define GENERATE_ENUM(e) e,
#define GENERATE_ENUM_STRING(e) #e,

#define ENUM_MACRO_DEFINE(enum_name, FOREACH_ENUM) \
enum enum_name \
{ \
	FOREACH_ENUM(GENERATE_ENUM) \
	enum_name##_MAX, \
}; \
extern const char* enum_name##String[enum_name##_MAX];

#define ENUM_MACRO_STRING(enum_name, FOREACH_ENUM) \
const char* enum_name##String[enum_name##_MAX] = { \
	FOREACH_ENUM(GENERATE_ENUM_STRING) \
};

// use macro
#define FOREACH_ENUM_2(_) \
	_(E2_A) \
	_(E2_B)
ENUM_MACRO_DEFINE(Enum2, FOREACH_ENUM_2)

#endif