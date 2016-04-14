#include <stdio.h>
#include "cmd_utils.h"
#include "str_utils.h"

int8_t ParseOption(const char* p_start, const char* p_end)
{
	MASSERT(p_start != p_end);

	char* p = StrUtils_AllocByDiff(p_start, p_end);
	char* q = p;
	int8_t ret_val = MG_OT_Null;
	int dash_count = 0;

	while (*q == '-')
	{
		++q;
		++dash_count;
	}

	if (strcmp(q, "Mode") == 0 && dash_count == 2)
	{
		ret_val = MG_OT_FollowOne;
	}
	else if (strcmp(q, "OpenGL") == 0 && dash_count == 2)
	{
		ret_val = MG_OT_Single;
	}
	else if (strcmp(q, "i") == 0 && dash_count == 1)
	{
		ret_val = MG_OT_FollowArbitrary;
	}

	free(p);

	return ret_val;
}

bool executeCommand(const char* cmd_name, const char** options, int option_num)
{
	int i;
	MLOG("Result of parse command line: \n");
	MLOG("command name: %s\n", cmd_name);
	for (i = 0; i < option_num; ++i)
	{
		MLOG("option: %s\n", options[i]);
	}
	MLOG("----------------\n");

	return true;
}

int main(int argc, char* argv[])
{
	// detect memory leak
#if MG_PLATFORM_WINDOWS && MUGGLE_DEBUG
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);
#endif	

	if (argc > 1)
	{
		size_t count_len = 0, len = 0;
		char *cmd_line = NULL, *p = NULL;
		int i;

		for (i = 0; i < argc; ++i)
		{
			count_len = count_len + strlen(argv[i]) + 1;
		}

		cmd_line = (char*)malloc(count_len);
		p = cmd_line;

		for (i = 0; i < argc; ++i)
		{
			len = strlen(argv[i]);
			memcpy(p, argv[i], len);
			p += len;
			*p = ' ';
			++p;
		}
		--p;
		*p = '\0';

		CmdUtils_ParseCmdLine(cmd_line, ParseOption, executeCommand);

		free(cmd_line);
	}
	else
	{
		char *cmd_line = "Test_CmdUtils  --Mode Test --OpenGL -i file1 file2 file3 -o=\"out1 out2 out3\"";
		CmdUtils_ParseCmdLine(cmd_line, ParseOption, executeCommand);
	}

	// detect memory leak
#if MG_PLATFORM_WINDOWS && MUGGLE_DEBUG
	_CrtMemCheckpoint(&s2);
	int mem_diff = _CrtMemDifference(&s3, &s1, &s2);
	if (mem_diff)
	{
		_CrtMemDumpStatistics(&s3);
		_CrtDumpMemoryLeaks();
	}
	MASSERT_MSG(mem_diff == 0, "memory leak!!!");
#endif

	return 0;
}