#include "yaml.h"
#include "muggle/c/muggle_c.h"
#include <stdio.h>

void parse_yaml_file(const char *filepath)
{
	yaml_parser_t parser;
	FILE *fp = NULL;

	if (!yaml_parser_initialize(&parser))
	{
		LOG_ERROR("Failed initialize yaml parser");
		return;
	}

	fp = fopen(filepath, "rb");
	if (fp == NULL)
	{
		LOG_ERROR("Failed open file: %s", filepath);
		goto parse_exit;
	}

	yaml_parser_set_input_file(&parser, fp);

	// TODO: do something here

parse_exit:
	yaml_parser_delete(&parser);

	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	if (argc <= 1)
	{
		LOG_ERROR("Usage: %s <yaml file>", argv[0]);
		exit(EXIT_FAILURE);
	}

	parse_yaml_file(argv[1]);

	return 0;
}
