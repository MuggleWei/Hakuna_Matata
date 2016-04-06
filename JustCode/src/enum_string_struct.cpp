#include "enum_string_struct.h"

// use common
const char* Enum1::EnumString[Enum1::Max] = {
	"A",
	"B",
};

// use macro
ENUM_STRUCT_STRING(Enum2, FOREACH_ENUM_2)