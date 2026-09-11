#include "foo_config.h"
#include "foo_log.h"

void run(foo_config_t *cfg)
{
	LOG_INFO("input arguments");
	LOG_INFO("host: %s", cfg->host);
	LOG_INFO("port: %s", cfg->port);
	LOG_INFO("log.console: %s", cfg->log_console_level);
	LOG_INFO("log.file: %s", cfg->log_file_level);
	LOG_INFO("log.use_rotate: %s", cfg->log_rotate ? "true" : "false");
}

int main(int argc, char *argv[])
{
	foo_config_t cfg;
	if (!parse_sys_args(argc, argv, &cfg)) {
		fprintf(stderr, "failed parse sys arguments\n");
		exit(EXIT_FAILURE);
	}

	if (!foo_log_init(cfg.log_console_level, cfg.log_file_level, "logs/foo.log",
					  false, NULL)) {
		fprintf(stderr, "failed init log\n");
		exit(EXIT_FAILURE);
	}

	foo_log_init_thread_ctx();

	run(&cfg);

	foo_log_cleanup_thread_ctx();

	return 0;
}
