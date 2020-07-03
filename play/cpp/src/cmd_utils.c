#include "cmd_utils.h"
#include "str_utils.h"

#define MG_MAX_CMD_OPTION_NUM 64

bool CmdUtils_ParseCmdLine(const char* cmd_line, ptrGetOptionType p_get_option_type, ptrCommandParseCallback p_callback)
{
	size_t cmd_len;
	char *cmd_buf, *p, *q, *p_end;
	char* options[MG_MAX_CMD_OPTION_NUM] = { NULL };
	int option_count = 0;
	char* p_command = NULL;

	// ensure get option function is not NULL
	if (p_get_option_type == NULL)
	{
		MASSERT_MSG(0, "Must assign parse option type function pointer");
		return false;
	}

	// copy command line
	cmd_len = strlen(cmd_line);
	cmd_buf = (char*)malloc(cmd_len + 1);
	memcpy(cmd_buf, cmd_line, cmd_len);
	cmd_buf[cmd_len] = '\0';
	q = p = cmd_buf;

	while (1)
	{
		// skip blank
		while (*p == ' ')
		{
			++p;
		}
		q = p;

		// read command or option name
		while (*p != '\0' && *p != ' ')
		{
			++p;
		}

		if (p == q)
		{
			break;
		}

		// record this command or option name
		if (p_command == NULL)
		{
			p_command = q;

			while (1)
			{
				if (*p == '\0' || *p == ' ')
				{
					break;
				}
			}

			if (*p == ' ')
			{
				*p = '\0';
				++p;
				q = p;
			}

			continue;
		}
		else
		{ 
			if (option_count == MG_MAX_CMD_OPTION_NUM - 1)
			{
				MASSERT_MSG(0, "Too many options, the max option number is %d", MG_MAX_CMD_OPTION_NUM);
				free(cmd_buf);
				return false;
			}
			options[option_count++] = q;
		}

		// parse
		if (*p == '\0')
		{
			if (q == p)
			{
				break;
			}

			p_end = CmdUtils_ParseOption(q, p-1, p_get_option_type);
			if (p_end == NULL)
			{
				MASSERT_MSG(0, "Failed in parse option: %s", q);
				free(cmd_buf);
				return false;
			}
			if (p_end - q < 0)
			{
				MASSERT_MSG(0, "Error in parse option: %s", q);
				free(cmd_buf);
				return false;
			}

			break;
		}
		else
		{
			p_end = CmdUtils_ParseOption(q, p-1, p_get_option_type);
			if (p_end == NULL)
			{
				*p = '\0';
				MASSERT_MSG(0, "Failed in parse option: %s", q);
				free(cmd_buf);
				return false;
			}

			++p_end;

			// parse complete
			if (*p_end == '\0')
			{
				break;
			}
			else
			{
				*p_end = '\0';
				p = p_end + 1;
			}
		}
	}

	// call back function
	if (p_callback)
	{
		(*p_callback)(p_command, options, option_count);
	}

	free(cmd_buf);

	return true;
}

char* CmdUtils_ParseOption(char* p_start, char* p_end, ptrGetOptionType p_get_option_type)
{
	char* p_ret = p_end;
	int8_t option_type;
	
	if (p_get_option_type == NULL)
	{
		MASSERT_MSG(0, "Must assign parse option type function pointer");
		return NULL;
	}
	if (p_end < p_start)
	{
		MASSERT_MSG(0, "End position must greater than start position");
		return NULL;
	}
	if (*p_start == '=')
	{
		MASSERT_MSG(0, "Option format is error");
		return NULL;
	}

	// detect format [option]="..."
	char* p = p_start;
	while (p != p_end)
	{
		if (*p == '=')
		{
			++p;
			if (*p == '\"')
			{
				++p;
				while (*p != '\0')
				{
					if (*p == '\"')
					{
						break;
					}
					++p;
				}

				if (*p == '\0')
				{
					return NULL;
				}

				option_type = MG_OT_WithArug;
				p_end = p;
				return p_end;
			}
			else
			{
				break;
			}
		}
		++p;
	}

	// get option type throught call back function
	option_type = (*p_get_option_type)(p_start, p_end);
	switch (option_type)
	{
	case MG_OT_Null:
	{
		p_ret = p_end;
		MASSERT_MSG(0, "Meaningless option");
		return false;
	}break;
	case MG_OT_Single:
	{
		p_ret = p_end;
	}break;
	case MG_OT_FollowOne:
	{
		p_ret = p_end + 1;

		// skip blank
		while (*p_ret == ' ')
		{
			if (*p_ret == '\0')
			{
				MASSERT_MSG(0, "parse [%s] Option type is wrong", p_start);
				return NULL;
			}
			++p_ret;
		}

		// get option argument
		while (1)
		{
			if (*p_ret != '\0' && *p_ret != ' ')
			{
				++p_ret;
			}
			else
			{
				break;
			}
		}

		--p_ret;
	}break;
	case MG_OT_FollowArbitrary:
	{
		p_ret = p_end;
		while (1)
		{
			if (*p_ret != '\0' && *p_ret != '-')
			{
				++p_ret;
			}
			else
			{
				if (*p_ret == '-')
				{
					--p_ret;
				}
				break;
			}
		}
		--p_ret;
	}break;
	}

	return p_ret;
}