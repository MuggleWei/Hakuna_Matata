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
#define CASE_ENUM_TO_STRING(e) case e: return #e;

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

#define ENUM_STRUCT(struct_name, FOREACH_ENUM) \
struct struct_name \
{ \
	enum Enum \
	{ \
		FOREACH_ENUM(GENERATE_ENUM) \
		Max, \
	}; \
	static const char* getEnumString(int index) \
	{ \
		switch (index) \
		{ \
			FOREACH_ENUM(CASE_ENUM_TO_STRING); \
		} \
		return ""; \
	} \
};

// use macro
#define FOREACH_ENUM_2(_) \
	_(A) \
	_(B)
ENUM_STRUCT_DEFINE(Enum2, FOREACH_ENUM_2)

// use macro
#define FOREACH_ENUM_3(_) \
	_(A) \
	_(B)
ENUM_STRUCT(Enum3, FOREACH_ENUM_3)

#endif