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

void on_token(yaml_token_t *p_token, muggle_stack_t *p_stack, int *p_status)
{
	int ident = muggle_stack_size(p_stack) * IDENT_WIDTH;
	switch (p_token->type)
	{
		// Stream start/end
		case YAML_STREAM_START_TOKEN:
		{
			printf("----------------\n");
			printf("STREAM START\n");
			printf("----------------\n");
		} break;
		case YAML_STREAM_END_TOKEN:
		{
			printf("----------------\n");
			printf("STREAM END\n");
			printf("----------------\n");
		} break;

		// Token types
		case YAML_KEY_TOKEN:
		{
			printf("%*c", ident, ' ');
			*p_status = STATUS_KEY;
		} break;
		case YAML_VALUE_TOKEN:
		{
			printf(": ");
			*p_status = STATUS_VALUE;
		} break;

		// Delimiters
		case YAML_BLOCK_ENTRY_TOKEN:
		{
			// printf("%*c***ENTRY TOKEN***\n", ident, ' ');
			*p_status = STATUS_NONE;
		} break;
		case YAML_BLOCK_MAPPING_START_TOKEN:
		{
			if (ident <= 0 || *p_status == STATUS_VALUE)
			{
				printf("{\n");
			}
			else
			{
				printf("%*c{\n", ident, ' ');
			}
			STACK_PUSH(p_stack, "{");
		} break;
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
		{
			printf("[\n");
			STACK_PUSH(p_stack, "[");
		} break;
		case YAML_FLOW_SEQUENCE_START_TOKEN:
		{
			printf("[\n");
			STACK_PUSH(p_stack, "[");
			*p_status = STATUS_NONE;
		} break;
		case YAML_FLOW_SEQUENCE_END_TOKEN:
		{
			char c = ']';
			muggle_stack_pop(p_stack, NULL, NULL);
			ident = muggle_stack_size(p_stack) * IDENT_WIDTH;
			if (ident > 0)
			{
				printf("%*c%c\n", ident, ' ', c);
			}
			else
			{
				printf("%c\n", c);
			}
		} break;
		case YAML_BLOCK_END_TOKEN:
		{
			muggle_stack_node_t *node = 
				(muggle_stack_node_t*)muggle_stack_top(p_stack);
			if (node == NULL)
			{
				LOG_ERROR("stack alreay empty!");
				break;
			}

			const char *data = (const char*)node->data;

			char c = '?';
			if (data[0] == '{')
			{
				c = '}';
			}
			if (data[0] == '[')
			{
				c = ']';
			}
			muggle_stack_pop(p_stack, NULL, NULL);
			ident = muggle_stack_size(p_stack) * IDENT_WIDTH;
			if (ident > 0)
			{
				printf("%*c%c\n", ident, ' ', c);
			}
			else
			{
				printf("%c\n", c);
			}
		} break;

		// Data
		case YAML_SCALAR_TOKEN:
		{
			if (*p_status == STATUS_NONE)
			{
				printf("%*c%s\n",
					ident, ' ',
					p_token->data.scalar.value);
			}
			else if (*p_status == STATUS_KEY)
			{
				printf("%s",
					p_token->data.scalar.value);
			}
			else if (*p_status == STATUS_VALUE)
			{
				printf("%s\n",
					p_token->data.scalar.value);
			}
		} break;
		default:
		{
			printf("Got token of type %d\n", p_token->type);
		} break;
	}
	fflush(stdout);
}

void run_parser(yaml_parser_t *p_parser)
{
	muggle_stack_t stack;
	muggle_stack_init(&stack, 2);

	int status = STATUS_NONE;

	yaml_token_t token;
	int done = 0;
	while (!done)
	{
		if (!yaml_parser_scan(p_parser, &token))
		{
			LOG_ERROR("failed parser scan");
			break;
		}

		on_token(&token, &stack, &status);

		done = (token.type == YAML_STREAM_END_TOKEN);

		yaml_token_delete(&token);
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
