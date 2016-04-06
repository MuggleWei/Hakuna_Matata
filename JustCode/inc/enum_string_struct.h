#ifndef __ENUM_STRING_STRUCT_H__
#define __ENUM_STRING_STRUCT_H__

// common enum
struct Enum1
{
	enum Enum
	{
		A,
		B,
		Max,
	};
	static const char* EnumString[Max];
};

// enum macro
#define GENERATE_ENUM(e) e,
#define GENERATE_ENUM_STRING(e) #e,

#define ENUM_STRUCT_DEFINE(struct_name, FOREACH_ENUM) \
struct struct_name \
{ \
	enum Enum \
	{ \
		FOREACH_ENUM(GENERATE_ENUM) \
		Max, \
	}; \
	static const char* EnumString[Max]; \
};
#define ENUM_STRUCT_STRING(struct_name, FOREACH_ENUM) \
const char* struct_name::EnumString[struct_name::Max] = { \
	FOREACH_ENUM(GENERATE_ENUM_STRING) \
};

// use macro
#define FOREACH_ENUM_2(_) \
	_(A) \
	_(B)
ENUM_STRUCT_DEFINE(Enum2, FOREACH_ENUM_2)

#endif