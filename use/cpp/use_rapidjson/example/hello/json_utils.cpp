#include "json_utils.h"

std::string JsonToString(const rapidjson::Value &val)
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
	val.Accept(writer);
	return std::string(buf.GetString());
}
