#include "enum_string.h"

// use common
const char* EnumString1[E1_MAX] = {
	"E1_A",
	"E1_B",
};

// use macro
ENUM_MACRO_STRING(Enum2, FOREACH_ENUM_2)