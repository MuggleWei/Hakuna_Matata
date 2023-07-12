#ifndef HELLO_JSON_UTILS_H_
#define HELLO_JSON_UTILS_H_

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

#define CHECK_RAPIDJSON_OBJECT(val, k) \
if (!val.HasMember(#k) || !val[#k].IsObject()) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with object type"); \
	throw std::logic_error(err_msg); \
}

#define CHECK_RAPIDJSON_STRING(val, k) \
if (!val.HasMember(#k) || !val[#k].IsString()) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with string type"); \
	throw std::logic_error(err_msg); \
}

#define CHECK_RAPIDJSON_INT64(val, k) \
if (!val.HasMember(#k) || !val[#k].IsInt64()) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with int64 type"); \
	throw std::logic_error(err_msg); \
}

#define CHECK_RAPIDJSON_UINT64(val, k) \
if (!val.HasMember(#k) || !val[#k].IsUint64()) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with int64 type"); \
	throw std::logic_error(err_msg); \
}

#define CHECK_RAPIDJSON_DOUBLE(val, k) \
if (!val.HasMember(#k) || !val[#k].IsDouble()) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with double type"); \
	throw std::logic_error(err_msg); \
}

#define CHECK_RAPIDJSON_DOUBLE_OR_INT64(val, k) \
if (!val.HasMember(#k) || (!val[#k].IsDouble() && !val[#k].IsInt64())) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with double or int64 type"); \
	throw std::logic_error(err_msg); \
}

#define CHECK_RAPIDJSON_ARRAY(val, k) \
if (!val.HasMember(#k) || !val[#k].IsArray()) \
{ \
	char err_msg[2048] = { 0 }; \
	snprintf(err_msg, sizeof(err_msg) - 1, "failed get field '" #k "' with array type"); \
	throw std::logic_error(err_msg); \
}

#define GET_RAPIDJSON_BOOL_WITH_DEFAULT(ret, val, k, default_v) \
if (val.HasMember(#k) && val[#k].IsBool()) \
{ \
	ret = val[#k].GetBool(); \
} \
else \
{ \
	is_resend = default_v; \
}

std::string JsonToString(const rapidjson::Value &val);

#endif // !HELLO_JSON_UTILS_H_
