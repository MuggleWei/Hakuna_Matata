#include "foo_config.h"

enum {
	OPT_START = 1000,
	OPT_LOG_CONSOLE_LEVEL,
	OPT_LOG_FILE_LEVEL,
	OPT_LOG_ROTATE,
};

bool parse_sys_args(int argc, char **argv, foo_config_t *cfg)
{
	const char *str_usage =
		"Usage: %s <options>\n"
		"    -h, --help         show man info\n"
		"    -v, --version      show version\n"
		"    -H, --host         listen/connect host\n"
		"    -P, --port         listen/connect port\n"
		"      , --log.console  log console level\n"
		"      , --log.file     log file level\n"
		"      , --log.rotate   log file use rotate\n"
		"\ne.g.\n"
		"  %s -H 127.0.0.1 -P 10102 "
		"--log.console=info --log.file=debug --log.rotate=1\n";

	memset(cfg, 0, sizeof(foo_config_t));

	// set default value
	strncpy(cfg->host, "0.0.0.0", sizeof(cfg->host) - 1);
	strncpy(cfg->port, "10102", sizeof(cfg->port) - 1);
	strncpy(cfg->log_console_level, "info", sizeof(cfg->log_console_level) - 1);
	strncpy(cfg->log_file_level, "debug", sizeof(cfg->log_file_level) - 1);
	cfg->log_rotate = false;

	int c = 0;
	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "help", no_argument, NULL, 'h' },
			{ "version", no_argument, NULL, 'v' },
			{ "host", required_argument, NULL, 'H' },
			{ "port", required_argument, NULL, 'P' },
			{ "log.console", required_argument, NULL, OPT_LOG_CONSOLE_LEVEL },
			{ "log.file", required_argument, NULL, OPT_LOG_FILE_LEVEL },
			{ "log.rotate", required_argument, NULL, OPT_LOG_ROTATE },
			{ NULL, 0, NULL, 0 }
		};

		c = getopt_long(argc, argv, "hvt:H:P:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			fprintf(stdout, str_usage, argv[0], argv[0]);
			exit(EXIT_SUCCESS);
		} break;
		case 'v': {
			fprintf(stdout, "mugglec: %s\n", MUGGLE_C_VERSION);
			fprintf(stdout, "foo: %s\n", FOO_VERSION);
			exit(EXIT_SUCCESS);
		} break;
		case 'H': {
			strncpy(cfg->host, optarg, sizeof(cfg->host) - 1);
		} break;
		case 'P': {
			strncpy(cfg->port, optarg, sizeof(cfg->port) - 1);
		} break;
		case OPT_LOG_CONSOLE_LEVEL: {
			strncpy(cfg->log_console_level, optarg,
					sizeof(cfg->log_console_level) - 1);
		} break;
		case OPT_LOG_FILE_LEVEL: {
			strncpy(cfg->log_file_level, optarg,
					sizeof(cfg->log_file_level) - 1);
		} break;
		case OPT_LOG_ROTATE: {
			cfg->log_rotate = atoi(optarg) == 0 ? false : true;
		} break;
		default: {
			fprintf(stderr, "unrecognized opt: %s", optarg);
		} break;
		}
	}

	return true;
}
