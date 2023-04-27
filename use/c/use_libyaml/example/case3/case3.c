#include "yaml.h"
#include "muggle/c/muggle_c.h"
#include <muggle/c/dsaa/stack.h>

#define IDENT_WIDTH 4

#define STATUS_NONE  0
#define STATUS_KEY   1
#define STATUS_VALUE 2

#define STACK_PUSH(stack, s) \
	int slen = strlen(s); \
	char *data = (char*)malloc(slen + 1); \
	strncpy(data, s, slen); \
	data[slen] = '\0'; \
	muggle_stack_push(stack, data);

void on_event(yaml_event_t *p_event, muggle_stack_t *p_stack, int *p_status)
{
	int ident = muggle_stack_size(p_stack) * IDENT_WIDTH;
	switch (p_event->type)
	{
		case YAML_NO_EVENT:
		{
			printf("No event!\n");
		} break;
		case YAML_STREAM_START_EVENT:
		{
			printf("----------------\n");
			printf("STREAM START\n");
			printf("----------------\n");
		} break;
		case YAML_STREAM_END_EVENT:
		{
			printf("----------------\n");
			printf("STREAM END\n");
			printf("----------------\n");
		} break;
		case YAML_DOCUMENT_START_EVENT:
		{
			printf("DOC START\n");
		} break;
		case YAML_DOCUMENT_END_EVENT:
		{
			printf("DOC END\n");
		} break;
		case YAML_SEQUENCE_START_EVENT:
		{
			if (*p_status == STATUS_VALUE)
			{
				printf("[\n");
			}
			else
			{
				printf("%*c[\n", ident, ' ');
			}
			STACK_PUSH(p_stack, "[");
			*p_status = STATUS_NONE;
		} break;
		case YAML_SEQUENCE_END_EVENT:
		{
			muggle_stack_pop(p_stack, NULL, NULL);
			ident = muggle_stack_size(p_stack) * IDENT_WIDTH;
			if (ident > 0)
			{
				printf("%*c]\n", ident, ' ');
			}
			else
			{
				printf("]\n");
			}
		} break;
		case YAML_MAPPING_START_EVENT:
		{
			if (*p_status == STATUS_VALUE)
			{
				printf("{\n");
			}
			else
			{
				printf("%*c{\n", ident, ' ');
			}
			STACK_PUSH(p_stack, "{");
			*p_status = STATUS_KEY;
		} break;
		case YAML_MAPPING_END_EVENT:
		{
			muggle_stack_pop(p_stack, NULL, NULL);
			ident = muggle_stack_size(p_stack) * IDENT_WIDTH;
			if (ident > 0)
			{
				printf("%*c}\n", ident, ' ');
			}
			else
			{
				printf("}\n");
			}
		} break;

		case YAML_ALIAS_EVENT:
		{
			printf("%*cALIAS: %s\n",
				ident, ' ',
				p_event->data.alias.anchor);
		} break;
		case YAML_SCALAR_EVENT:
		{
			muggle_stack_node_t *node = muggle_stack_top(p_stack);
			if (node == NULL)
			{
				printf("%*c%s: ",
					ident, ' ',
					p_event->data.scalar.value);
				*p_status = STATUS_VALUE;
			}
			else
			{
				const char *data = (const char*)node->data;
				if (data[0] == '{')
				{
					if (*p_status == STATUS_KEY || *p_status == STATUS_NONE)
					{
						printf("%*c%s: ",
							ident, ' ',
							p_event->data.scalar.value);
						*p_status = STATUS_VALUE;
					}
					else
					{
						printf("%s\n", p_event->data.scalar.value);
						*p_status = STATUS_KEY;
					}
				}
				else if (data[0] == '[')
				{
					printf("%*c%s\n",
						ident, ' ',
						p_event->data.scalar.value);
				}
			}
		} break;
	}
}

void run_parser(yaml_parser_t *p_parser)
{
	muggle_stack_t stack;
	muggle_stack_init(&stack, 2);

	int status = STATUS_NONE;

	yaml_event_t event;
	int done = 0;
	while (!done)
	{
		if (!yaml_parser_parse(p_parser, &event))
		{
			LOG_ERROR("failed parser scan");
			break;
		}

		on_event(&event, &stack, &status);

		done = (event.type == YAML_STREAM_END_EVENT);

		yaml_event_delete(&event);
	}

	muggle_stack_destroy(&stack, NULL, NULL);
}

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

	run_parser(&parser);

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

