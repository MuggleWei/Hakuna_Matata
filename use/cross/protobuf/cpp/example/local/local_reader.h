#include <google/protobuf/message.h>

#include "msg_handle/codec.h"
#include "msg_handle/msg_handler.h"
#include "gen/proto/gameobject.pb.h"
#include "gen/proto/food.pb.h"

using namespace google::protobuf;
using namespace gen::proto;

class LocalReader
{
public:
	void OnMessage(std::shared_ptr<Message> &msg);
	void OnMessagePosition(std::shared_ptr<Position> &msg);
	void OnMessageRotation(std::shared_ptr<Rotation> &msg);
	void OnMessageTransform(std::shared_ptr<Transform> &msg);
	void OnMessageLunch(std::shared_ptr<Lunch> &msg);

	void Register();

	int32_t Parse(char *bytes);

private:
	Codec codec_;
	ProtoMessageHandler handler_;
};
