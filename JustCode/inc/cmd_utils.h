#ifndef __MUGGLE_CMD_UTILS_H__
#define __MUGGLE_CMD_UTILS_H__

#include "base.h"

EXTERN_C_BEGIN

enum MG_eOptionType
{
	MG_OT_Null = 0,			// do nothing
	MG_OT_Single,			// single option without argument
	MG_OT_FollowOne,		// option followed by one argument 
	MG_OT_FollowArbitrary,	// option followed by arbitrary number of arguments
	MG_OT_WithArug,			// option="*"
};

/*
 *  typedef parse command call back function 
 *  @cmd_name : command name
 *  @options : the start of option pointer array, the end is NULL
 */
typedef bool (*ptrCommandParseCallback)(const char* cmd_name, const char** options, int option_num);

/*
 *  get option type
 *  @p_start: the first character position
 *  @p_end: last character in option name
 *  @return value: enumeration in MG_eOptionType
 */
typedef int8_t (*ptrGetOptionType)(const char* p_start, const char* p_end);

/*
 *  parse command line
 *  command line format: command [option]
 *  option format: 
 *		1. -xxx/--yyy="xxx"
 *		2. -xxx/--yyy f (single option argument)
 *		2. -xxx/--xxx file1 file2 (arbitrary number of option argument must be end with next option or end of command line) 
 *  
 *  @cmd_line : command line
 *  @p_get_option_type : get option type function pointer
 *  @p_callback : complete command parse, call this execute command line
 */
MG_DLL bool CmdUtils_ParseCmdLine(const char* cmd_line, ptrGetOptionType p_get_option_type, ptrCommandParseCallback p_callback);
/*
 *  parse option name and return entire option end
 *  @p_start: the first character position
 *  @p_end: last character in option name
 *  @return value: last character in entire option (option name and option arguments)
 */
MG_DLL char* CmdUtils_ParseOption(char* p_start, char* p_end, ptrGetOptionType p_get_option_type);

EXTERN_C_END

#endif