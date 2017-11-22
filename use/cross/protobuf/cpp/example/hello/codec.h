#ifndef CODEC_H_
#define CODEC_H_

#include <google/protobuf/message.h>

class Codec
{
public:
	char* Serialize(const google::protobuf::Message *msg);
	google::protobuf::Message* Deserialize(const char *bytes);
};

#endif