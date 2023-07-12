#include "muggle/cpp/muggle_cpp.h"
#include "json_utils.h"

bool parse_json(rapidjson::Document *p_doc)
{
	LOG_INFO("start parse json");

	rapidjson::Document &doc = *p_doc;

	// string
	CHECK_RAPIDJSON_STRING(doc, desc);
	const char *desc = doc["desc"].GetString();
	LOG_INFO("\"%s\": \"%s\"", "desc", desc);

	// array
	CHECK_RAPIDJSON_ARRAY(doc, array);
	for (size_t i = 0; i < doc["array"].Size(); ++i) {
		rapidjson::Value &item = doc["array"][(rapidjson::SizeType)i];
		CHECK_RAPIDJSON_UINT64(item, id);
		CHECK_RAPIDJSON_STRING(item, name);

		uint64_t id = item["id"].GetUint64();
		const char *name = item["name"].GetString();
		LOG_INFO("array[%u]: { \"id\": %llu, \"name\": \"%s\" }",
				 (unsigned int)i, (unsigned long long)id, name);
	}

	// object
	CHECK_RAPIDJSON_OBJECT(doc, object);
	rapidjson::Value &obj = doc["object"];

	CHECK_RAPIDJSON_UINT64(obj, id);
	CHECK_RAPIDJSON_STRING(obj, name);

	uint64_t id = obj["id"].GetUint64();
	const char *name = obj["name"].GetString();
	LOG_INFO("object: { \"id\": %llu, \"name\": \"%s\" }",
			 (unsigned long long)id, name);

	return true;
}

bool load_json_file(const char *filepath)
{
	LOG_INFO("load json file: %s", filepath);

	FILE *fp = nullptr;
	char *buf = nullptr;
	rapidjson::Document *doc = nullptr;

	fp = fopen(filepath, "rb");
	if (fp == nullptr) {
		LOG_ERROR("failed open json file: %s", filepath);
		return false;
	}

	bool ret = true;
	try {
		fseek(fp, 0, SEEK_END);
		long numbytes = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		buf = (char *)malloc((size_t)numbytes + 1);
		memset(buf, 0, numbytes + 1);
		size_t cnt_read = fread(buf, 1, numbytes, fp);
		if (cnt_read != (size_t)numbytes) {
			throw std::runtime_error("failed read json file");
		}

		doc = new rapidjson::Document;
		rapidjson::ParseResult ok = doc->Parse(buf);
		if (!ok) {
			char errmsg[2048];
			snprintf(errmsg, sizeof(errmsg),
					 "failed parse json, offset: %u, errmsg: %s",
					 (unsigned int)ok.Offset(),
					 rapidjson::GetParseError_En(ok.Code()));
		}

		parse_json(doc);
	} catch (std::exception const &e) {
		LOG_ERROR("failed parse json file: %s, exception: %s", e.what());
		ret = false;
	}

	if (doc) {
		delete doc;
	}

	if (buf) {
		free(buf);
	}

	if (fp) {
		fclose(fp);
	}

	return ret;
}

void response(rapidjson::Document *doc)
{
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);

	writer.StartObject();
	writer.Key("err_id");
	writer.Int(0);
	writer.Key("err_msg");
	writer.String("");
	if (doc) {
		writer.Key("data");
		doc->Accept(writer);
	}
	writer.EndObject();

	LOG_INFO("response json: %s", s.GetString());
}

void write_json()
{
	try {
		rapidjson::Document doc;
		doc.SetObject();

		// string
		rapidjson::Value v_desc("hello world", doc.GetAllocator());
		doc.AddMember("desc", v_desc, doc.GetAllocator());

		// array
		rapidjson::Value arr(rapidjson::kArrayType);
		{
			rapidjson::Value obj(rapidjson::kObjectType);
			obj.AddMember("id", 0, doc.GetAllocator());
			rapidjson::Value v_name("foo", doc.GetAllocator());
			obj.AddMember("name", v_name, doc.GetAllocator());
			arr.PushBack(obj, doc.GetAllocator());
		}
		{
			rapidjson::Value obj(rapidjson::kObjectType);
			obj.AddMember("id", 1, doc.GetAllocator());
			rapidjson::Value v_name("bar", doc.GetAllocator());
			obj.AddMember("name", v_name, doc.GetAllocator());
			arr.PushBack(obj, doc.GetAllocator());
		}
		doc.AddMember("array", arr, doc.GetAllocator());

		// object
		rapidjson::Value obj(rapidjson::kObjectType);
		obj.AddMember("id", 2, doc.GetAllocator());
		rapidjson::Value v_name("baz", doc.GetAllocator());
		obj.AddMember("name", v_name, doc.GetAllocator());
		doc.AddMember("object", obj, doc.GetAllocator());

		std::string s = JsonToString(doc);
		LOG_INFO("generate json: %s", s.c_str());

		response(&doc);
	} catch (std::exception const &e) {
		LOG_ERROR("failed write json: %s, exception: %s", e.what());
	}
}

int main()
{
	muggle::Log::ComplicatedInit(LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, NULL);

	// example read
	const char *filepath = "./etc/hello.json";
	if (!load_json_file(filepath)) {
		LOG_ERROR("failed parse json file: %s", filepath);
		exit(EXIT_FAILURE);
	}

	// example write
	write_json();

	return 0;
}
