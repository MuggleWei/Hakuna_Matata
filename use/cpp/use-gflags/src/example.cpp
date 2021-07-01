#include <stdio.h>
#include "gflags/gflags.h"

DEFINE_string(config, "config/xxx.cfg", "config path");
DEFINE_int64(arg_int, 0, "argument int64");

int main(int argc, char *argv[])
{
	gflags::SetUsageMessage("%s --config config_path --arg_int int64_argument");
	gflags::SetVersionString("1.0.1");

	gflags::ParseCommandLineFlags(&argc, &argv, true);

	// detect "config" is set
	gflags::CommandLineFlagInfo info;
	if (GetCommandLineFlagInfo("config", &info) && info.is_default)
	{
		printf("config not set\n");
	}
	else
	{
		printf("config is set\n");
	}

	printf("config path: %s\n", FLAGS_config.c_str());
	printf("arg_int: %lld\n", (long long)FLAGS_arg_int);

	gflags::ShutDownCommandLineFlags();
	printf("bye bye\n");

	return 0;
}
