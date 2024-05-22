#include "pfpa_log_handle.h"
#include "muggle/c/muggle_c.h"

static int pfpa_log_init_fmt(const muggle_log_msg_t *msg, char *buf,
							 size_t bufsize)
{
	const char *level = muggle_log_level_to_str(msg->level);

	char filename[MUGGLE_MAX_PATH];
	muggle_path_basename(msg->src_loc.file, filename, sizeof(filename));

	unsigned long long tid = muggle_thread_current_readable_id();

	struct tm t;
	gmtime_r(&msg->ts.tv_sec, &t);

	const char *payload = "";
	if (msg->payload) {
		payload = msg->payload;
	}

	return (int)snprintf(
		buf, bufsize,
		"%s|%d-%02d-%02dT%02d:%02d:%02d.%09d|%s:%u|%s|%llu - %s\n", level,
		(int)t.tm_year + 1900, (int)t.tm_mon + 1, (int)t.tm_mday,
		(int)t.tm_hour, (int)t.tm_min, (int)t.tm_sec, (int)msg->ts.tv_nsec,
		filename, (unsigned int)msg->src_loc.line, msg->src_loc.func,
		(unsigned long long)tid, payload);
}

bool pfpa_log_init(int console_level, int file_level, const char *filepath)
{
	static muggle_log_fmt_t formatter = { MUGGLE_LOG_FMT_LEVEL |
											  MUGGLE_LOG_FMT_FILE |
											  MUGGLE_LOG_FMT_FUNC |
											  MUGGLE_LOG_FMT_TIME,
										  pfpa_log_init_fmt };
	muggle_logger_t *logger = muggle_logger_default();

	if (console_level >= 0) {
		static muggle_log_console_handler_t console_handler;
		muggle_log_console_handler_init(&console_handler, 1);
		muggle_log_handler_set_level((muggle_log_handler_t *)&console_handler,
									 console_level);
		muggle_log_handler_set_fmt((muggle_log_handler_t *)&console_handler,
								   &formatter);

		logger->add_handler(logger, (muggle_log_handler_t *)&console_handler);
	}

	if (file_level >= 0) {
		static muggle_log_file_handler_t file_handler;
		int ret = muggle_log_file_handler_init(&file_handler, filepath, "a");
		if (ret != 0) {
			fprintf(stderr, "failed init file handler with path: %s\n",
					filepath);
		}

		muggle_log_handler_set_level((muggle_log_handler_t *)&file_handler,
									 file_level);
		muggle_log_handler_set_fmt((muggle_log_handler_t *)&file_handler,
								   &formatter);
		logger->add_handler(logger, (muggle_log_handler_t *)&file_handler);
	}

	return true;
}
