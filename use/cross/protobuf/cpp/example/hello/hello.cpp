#include <stdio.h>
#include <iostream>
#include <map>
#include <functional>
#include <google/protobuf/message.h>
#include "gen/proto/gameobject.pb.h"
#include "gen/proto/food.pb.h"
#include "codec.h"
#include "msg_handler.h"

using namespace google::protobuf;
using namespace gen::proto;

class Receiver
{
public:
	void OnMessage(std::shared_ptr<Message> &msg)
	{
		std::cout << std::endl << "========" << std::endl;
		std::cout << "on message: " << msg->GetTypeName() << std::endl;
	}

	void OnMessagePosition(std::shared_ptr<Position> &msg)
	{
		std::cout << std::endl << "========" << std::endl;
		std::cout << "OnMessagePosition: " << std::endl
			<< msg->x() << ", "
			<< msg->y() << ", "
			<< msg->z() << std::endl;
	}
	void OnMessageRotation(std::shared_ptr<Rotation> &msg)
	{
		std::cout << std::endl << "========" << std::endl;
		std::cout << "OnMessageRotation: " << std::endl
			<< msg->x_axis() << ", "
			<< msg->y_axis() << ", "
			<< msg->z_axis() << std::endl;
	}
	void OnMessageTransform(std::shared_ptr<Transform> &msg)
	{
		std::cout << std::endl << "========" << std::endl;
		std::cout << "OnMessageTransform: " << std::endl
			<< msg->position().x() << ", "
			<< msg->position().y() << ", "
			<< msg->position().z() << std::endl
			<< msg->rotation().x_axis() << ", "
			<< msg->rotation().y_axis() << ", "
			<< msg->rotation().z_axis() << std::endl
			<< msg->scale().x_scale() << ", "
			<< msg->scale().y_scale() << ", "
			<< msg->scale().z_scale() << std::endl;
	}
	void OnMessageLunch(std::shared_ptr<Lunch> &msg)
	{
		std::cout << std::endl << "========" << std::endl;
		std::cout << "OnMessageLunch: " << std::endl;
		for (int i = 0; i < msg->foods_size(); ++i)
		{
			switch (msg->foods(i).food_oneof_case())
			{
			case Food::FoodOneofCase::kRice:
			{
				auto rice = msg->foods(i).rice();
				std::cout << i << ") rice: " << rice.origin_place() << std::endl;
			}break;
			case Food::FoodOneofCase::kDumpling:
			{
				auto dumpling = msg->foods(i).dumpling();
				std::cout << i << ") dumpling: " << dumpling.stuffing() << ", " << dumpling.taste() << std::endl;
			}break;
			case Food::FoodOneofCase::kNoodle:
			{
				auto noodle = msg->foods(i).noodle();
				std::cout << i << ") noodle: " << noodle.material() << ", " << noodle.taste() << std::endl;
			}break;
			case Food::FoodOneofCase::FOOD_ONEOF_NOT_SET:
			{
				std::cout << i << ") not set: " << std::endl;
			}break;
			}
		}
	}

	void Register()
	{
		handler_.RegisterDefault<Message>(std::bind(&Receiver::OnMessage, this, std::placeholders::_1));
		
		handler_.Register<Position>(std::bind(&Receiver::OnMessagePosition, this, std::placeholders::_1));
		handler_.Register<Rotation>(std::bind(&Receiver::OnMessageRotation, this, std::placeholders::_1));
		handler_.Register<Transform>(std::bind(&Receiver::OnMessageTransform, this, std::placeholders::_1));
		handler_.Register<Lunch>(std::bind(&Receiver::OnMessageLunch, this, std::placeholders::_1));
	}

	int32_t Parse(char *bytes)
	{
		int32_t total_len = 0;
		Message *msg = codec_.Deserialize(bytes, total_len);
		if (msg)
		{
			auto sp = std::shared_ptr<Message>(msg);
			handler_.OnMessage(sp);
		}

		return total_len;
	}

private:
	Codec codec_;
	ProtoMessageHandler handler_;
};

void baseSample()
{
	Receiver receiver;
	Codec codec;
	char *bytes = nullptr;
	int32_t total_len = 0;

	receiver.Register();

	// position
	Position pos;
	pos.set_x(5.0);
	pos.set_y(1.0);
	pos.set_z(6.0);
	bytes = codec.Serialize(&pos, total_len);
	receiver.Parse(bytes);
	free(bytes);

	// rotation
	Rotation rot;
	rot.set_x_axis(3.14f);
	rot.set_y_axis(0.0f);
	rot.set_z_axis(1.57f);
	bytes = codec.Serialize(&rot, total_len);
	receiver.Parse(bytes);
	free(bytes);

	// scale
	Scale scale;
	scale.set_x_scale(1.0f);
	scale.set_y_scale(1.0f);
	scale.set_z_scale(1.0f);
	bytes = codec.Serialize(&scale, total_len);
	receiver.Parse(bytes);
	free(bytes);

	// transform
	Transform transform;
	*transform.mutable_position() = pos;
	*transform.mutable_rotation() = rot;
	bytes = codec.Serialize(&transform, total_len);
	receiver.Parse(bytes);
	free(bytes);

	// lunch
	Lunch lunch;

	lunch.add_foods()->mutable_rice()->set_origin_place("fujian");

	auto noddle = lunch.add_foods()->mutable_noodle();
	noddle->set_material("flour");
	noddle->set_taste(Taste::NO_SPICY);

	auto dumpling = lunch.add_foods()->mutable_dumpling();
	dumpling->set_stuffing("Leek pork");
	dumpling->set_taste(Taste::SPICY);

	lunch.add_foods();

	bytes = codec.Serialize(&lunch, total_len);
	receiver.Parse(bytes);
	free(bytes);
}

void writeSample()
{
	Codec codec;
	char *bytes = nullptr;
	int32_t total_len = 0;

	FILE *fp = fopen("msg_bytes", "wb");
	if (fp == nullptr)
	{
		return;
	}

	// position
	Position pos;
	pos.set_x(5.0);
	pos.set_y(1.0);
	pos.set_z(6.0);
	bytes = codec.Serialize(&pos, total_len);
	fwrite(bytes, 1, total_len, fp);
	free(bytes);

	// rotation
	Rotation rot;
	rot.set_x_axis(3.14f);
	rot.set_y_axis(0.0f);
	rot.set_z_axis(1.57f);
	bytes = codec.Serialize(&rot, total_len);
	fwrite(bytes, 1, total_len, fp);
	free(bytes);

	// scale
	Scale scale;
	scale.set_x_scale(1.0f);
	scale.set_y_scale(1.0f);
	scale.set_z_scale(1.0f);
	bytes = codec.Serialize(&scale, total_len);
	fwrite(bytes, 1, total_len, fp);
	free(bytes);

	// transform
	Transform transform;
	*transform.mutable_position() = pos;
	*transform.mutable_rotation() = rot;
	bytes = codec.Serialize(&transform, total_len);
	fwrite(bytes, 1, total_len, fp);
	free(bytes);

	// lunch
	Lunch lunch;

	lunch.add_foods()->mutable_rice()->set_origin_place("fujian");

	auto noddle = lunch.add_foods()->mutable_noodle();
	noddle->set_material("flour");
	noddle->set_taste(Taste::NO_SPICY);

	auto dumpling = lunch.add_foods()->mutable_dumpling();
	dumpling->set_stuffing("Leek pork");
	dumpling->set_taste(Taste::SPICY);

	lunch.add_foods();

	bytes = codec.Serialize(&lunch, total_len);
	fwrite(bytes, 1, total_len, fp);
	free(bytes);

	fclose(fp);
}

void readSample()
{
	Receiver receiver;
	receiver.Register();

	FILE *fp = fopen("msg_bytes", "rb");
	if (fp == nullptr)
	{
		return;
	}

	fseek(fp, 0, SEEK_END);
	long file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	void *bytes = malloc(file_len);
	fread(bytes, file_len, 1, fp);

	int32_t accum_len = 0;
	while (true)
	{
		char *p = (char*)bytes + accum_len;
		int32_t parse_len = receiver.Parse(p);
		accum_len += parse_len;
		if (accum_len >= file_len)
		{
			free(bytes);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		baseSample();
	}
	else if (argc >= 2)
	{
		if (argv[1][0] == 'r')
		{
			readSample();
		}
		else if (argv[1][0] == 'w')
		{
			writeSample();
		}
		else
		{
			std::cout << "enter: " << argv[0] << " [r|w]" << std::endl;
		}
	}

	return 0;
}
