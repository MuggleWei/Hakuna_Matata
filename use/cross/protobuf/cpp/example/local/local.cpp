#include <stdio.h>
#include <iostream>
#include <map>
#include <functional>
#include <google/protobuf/message.h>
#include "muggle/c/muggle_c.h"
#include "local_reader.h"

using namespace google::protobuf;
using namespace gen::proto;

void localSample()
{
	LocalReader reader;
	Codec codec;
	char *bytes = nullptr;
	int32_t total_len = 0;

	reader.Register();

	// position
	Position pos;
	pos.set_x(5.0);
	pos.set_y(1.0);
	pos.set_z(6.0);
	bytes = codec.Serialize(&pos, total_len);
	reader.Parse(bytes);
	free(bytes);

	// rotation
	Rotation rot;
	rot.set_x_axis(3.14f);
	rot.set_y_axis(0.0f);
	rot.set_z_axis(1.57f);
	bytes = codec.Serialize(&rot, total_len);
	reader.Parse(bytes);
	free(bytes);

	// scale
	Scale scale;
	scale.set_x_scale(1.0f);
	scale.set_y_scale(1.0f);
	scale.set_z_scale(1.0f);
	bytes = codec.Serialize(&scale, total_len);
	reader.Parse(bytes);
	free(bytes);

	// transform
	Transform transform;
	*transform.mutable_position() = pos;
	*transform.mutable_rotation() = rot;
	bytes = codec.Serialize(&transform, total_len);
	reader.Parse(bytes);
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
	reader.Parse(bytes);
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
	LocalReader reader;
	reader.Register();

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
		int32_t parse_len = reader.Parse(p);
		accum_len += parse_len;
		if (accum_len >= file_len)
		{
			free(bytes);
			break;
		}
	}

	fclose(fp);
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(LOG_LEVEL_INFO, -1, nullptr);

	char usage_str[512];
	snprintf(usage_str, sizeof(usage_str), "\nUsage: %s <l|r|w>\n"
		"  l: local\n"
		"  r: read\n"
		"  w: write", argv[0]);

	char mode = 'l';
	if (argc != 2)
	{
		LOG_INFO(usage_str);
		LOG_INFO("without specify mode, use 'l'");
	}
	else
	{
		mode = argv[1][0];
	}

	switch (mode)
	{
		case 'l': localSample(); break;
		case 'r': readSample(); break;
		case 'w': writeSample(); break;
		case 'h': LOG_INFO(usage_str); break;
		default:
		{
			LOG_ERROR("invalid mode '%c'", mode);
			LOG_ERROR(usage_str);
		} break;
	}

	return 0;
}
