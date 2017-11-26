#include "codec.h"
#if WIN32
#include <WinSock2.h>
#include <malloc.h>

#pragma comment(lib,"ws2_32.lib")
#else
#include <arpa/inet.h>
#include <alloca.h>
#endif
#include <string.h>
#include <google/protobuf/io/coded_stream.h>

static google::protobuf::Message* CreateProtoMessage(const std::string &message_name)
{
	auto gen_pool = google::protobuf::DescriptorPool::generated_pool();
	const google::protobuf::Descriptor *desc = gen_pool->FindMessageTypeByName(message_name);
	if (desc == nullptr)
	{
		return nullptr;
	}
	return google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc)->New();
}

char* Codec::Serialize(const google::protobuf::Message *msg, int32_t &total_len)
{
	/*
	 |          int32_t               |      16 byte     |     int32_t    |   char*   |    void*   |
	 |   total_len(include itself)    |   version info   |    name_len    |   name    |    data    |
	 */

	static char *version = "0.0.1";
	size_t size = msg->ByteSizeLong();
	const std::string &type_name = msg->GetDescriptor()->full_name();
	total_len = (int32_t)(sizeof(int32_t) + 16 + sizeof(int32_t) + type_name.size() + size);

	void* bytes = malloc(total_len);
	if (!bytes)
	{
		return nullptr;
	}

	char *p = (char*)bytes;

	// total len
	*((int32_t*)p) = (int32_t)htonl(total_len);
	p += sizeof(int32_t);

	// version info
	memset(p, 0, 16);
	strncpy(p, version, 15);
	p += 16;

	// name len
	*((int32_t*)p) = (int32_t)htonl(type_name.size());
	p += sizeof(int32_t);

	// name
	memcpy(p, type_name.c_str(), type_name.size());
	p += (type_name.size());

	// data
	if (!msg->SerializeToArray(p, (int)size))
	{
		free(bytes);
		bytes = nullptr;
	}
	return (char*)bytes;
}

google::protobuf::Message* Codec::Deserialize(const char *bytes, int32_t &total_len)
{
	static const int32_t limit_name_len = 129;

	const char *p = bytes;
	char name[limit_name_len] = { 0 };

	// total len
	total_len = (int32_t)ntohl(*(int32_t*)p);
	p += (int32_t)sizeof(int32_t);

	// version info
	char version[16] = { 0 };
	memcpy(version, p, 15);
	p += 16;

	// name len
	int32_t name_len = (int32_t)ntohl(*(int32_t*)p);
	if (name_len <= 0 || name_len >= limit_name_len)
	{
		return nullptr;
	}
	p += sizeof(int32_t);

	// name
	memcpy(name, p, name_len);
	google::protobuf::Message *msg = CreateProtoMessage(name);
	if (msg == nullptr)
	{
		return nullptr;
	}
	p += name_len;

	// data
	int size = total_len - sizeof(int32_t) - 16 - sizeof(int32_t) - name_len;
	if (!msg->ParseFromArray(p, size))
	{
		delete msg;
		return nullptr;
	}

	return msg;
}