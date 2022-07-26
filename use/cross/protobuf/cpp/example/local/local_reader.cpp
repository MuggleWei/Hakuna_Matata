#include "local_reader.h"
#include "muggle/c/muggle_c.h"

void LocalReader::OnMessage(std::shared_ptr<Message> &msg)
{
	LOG_INFO("========================");
	LOG_INFO("on message: %s; (no callback function specified, use default callback)", msg->GetTypeName().c_str());
}

void LocalReader::OnMessagePosition(std::shared_ptr<Position> &msg)
{
	LOG_INFO("========================");
	LOG_INFO("OnMessagePosition: (%f, %f, %f)",
		msg->x(), msg->y(), msg->z());
}
void LocalReader::OnMessageRotation(std::shared_ptr<Rotation> &msg)
{
	LOG_INFO("========================");
	LOG_INFO("OnMessageRotation: (%f, %f, %f)",
		msg->x_axis(), msg->y_axis(), msg->z_axis());
}
void LocalReader::OnMessageTransform(std::shared_ptr<Transform> &msg)
{
	LOG_INFO("========================");
	LOG_INFO("OnMessageTransform:\n"
		"position: (%f, %f, %f)\n"
		"rotation: (%f, %f, %f)\n"
		"scale: (%f, %f, %f)",
		msg->position().x(), msg->position().y(), msg->position().z(),
		msg->rotation().x_axis(), msg->rotation().y_axis(), msg->rotation().z_axis(),
		msg->scale().x_scale(), msg->scale().y_scale(), msg->scale().z_scale());
}
void LocalReader::OnMessageLunch(std::shared_ptr<Lunch> &msg)
{
	LOG_INFO("========================");
	LOG_INFO("OnMessageLunch:");
	for (int i = 0; i < msg->foods_size(); ++i)
	{
		switch (msg->foods(i).food_oneof_case())
		{
			case Food::FoodOneofCase::kRice:
				{
					auto rice = msg->foods(i).rice();
					LOG_INFO("%d) rice: %s", i, rice.origin_place().c_str());
				}break;
			case Food::FoodOneofCase::kDumpling:
				{
					auto dumpling = msg->foods(i).dumpling();
					const std::string &taste = Taste_Name(dumpling.taste());
					LOG_INFO("%d) dumpling: %s, %s",
						i, dumpling.stuffing().c_str(), taste.c_str());
				}break;
			case Food::FoodOneofCase::kNoodle:
				{
					auto noodle = msg->foods(i).noodle();
					const std::string &taste = Taste_Name(noodle.taste());
					LOG_INFO("%d) noodle: %s, %s",
						i, noodle.material().c_str(), taste.c_str());
				}break;
			case Food::FoodOneofCase::FOOD_ONEOF_NOT_SET:
				{
					LOG_INFO("%d) not set:", i);
				}break;
		}
	}
}

void LocalReader::Register()
{
	handler_.RegisterDefault<Message>(std::bind(&LocalReader::OnMessage, this, std::placeholders::_1));

	handler_.Register<Position>(std::bind(&LocalReader::OnMessagePosition, this, std::placeholders::_1));
	handler_.Register<Rotation>(std::bind(&LocalReader::OnMessageRotation, this, std::placeholders::_1));
	handler_.Register<Transform>(std::bind(&LocalReader::OnMessageTransform, this, std::placeholders::_1));
	handler_.Register<Lunch>(std::bind(&LocalReader::OnMessageLunch, this, std::placeholders::_1));
}

int32_t LocalReader::Parse(char *bytes)
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
