#include "foo_log.h"

#if FOO_USE_SYNC_LOG

static int sync_log_complicated_init_fmt(const muggle_log_msg_t *msg, char *buf,
										 size_t bufsize)
{
	const char *level = muggle_log_level_to_str(msg->level);

	char filename[MUGGLE_MAX_PATH];
	muggle_path_basename(msg->src_loc.file, filename, sizeof(filename));

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
		(unsigned long long)msg->tid, payload);
}

static int foo_sync_log_str2level(const char *s)
{
	if (s == NULL) {
		return -1;
	}

	if (strcmp(s, "TRACE") == 0 || strcmp(s, "trace") == 0) {
		return MUGGLE_LOG_LEVEL_TRACE;
	}
	if (strcmp(s, "DEBUG") == 0 || strcmp(s, "debug") == 0) {
		return MUGGLE_LOG_LEVEL_DEBUG;
	}
	if (strcmp(s, "INFO") == 0 || strcmp(s, "info") == 0) {
		return MUGGLE_LOG_LEVEL_INFO;
	}
	if (strcmp(s, "WARNING") == 0 || strcmp(s, "warning") == 0 ||
		strcmp(s, "WARN") == 0 || strcmp(s, "warn") == 0) {
		return MUGGLE_LOG_LEVEL_WARNING;
	}
	if (strcmp(s, "ERROR") == 0 || strcmp(s, "error") == 0) {
		return MUGGLE_LOG_LEVEL_ERROR;
	}
	if (strcmp(s, "FATAL") == 0 || strcmp(s, "fatal") == 0) {
		return MUGGLE_LOG_LEVEL_FATAL;
	}
	return -1;
}

static bool foo_sync_log_init(const char *str_console_level,
							  const char *str_file_level, const char *filepath,
							  bool use_rotate, before_log_run_callback fn)
{
	static muggle_log_fmt_t formatter = {
		MUGGLE_LOG_FMT_LEVEL | MUGGLE_LOG_FMT_FILE | MUGGLE_LOG_FMT_FUNC |
			MUGGLE_LOG_FMT_TIME | MUGGLE_LOG_FMT_THREAD,
		sync_log_complicated_init_fmt
	};
	muggle_logger_t *logger = muggle_logger_default();

	int level_console = foo_sync_log_str2level(str_console_level);
	int level_file = foo_sync_log_str2level(str_file_level);

	if (level_console >= 0) {
		static muggle_log_console_handler_t console_handler;
		muggle_log_console_handler_init(&console_handler, 1);
		muggle_log_handler_set_level((muggle_log_handler_t *)&console_handler,
									 level_console);
		muggle_log_handler_set_fmt((muggle_log_handler_t *)&console_handler,
								   &formatter);

		logger->add_handler(logger, (muggle_log_handler_t *)&console_handler);
	}

	if (level_file >= 0) {
		if (use_rotate) {
			static muggle_log_file_time_rot_handler_t rot_file_handler;
			int ret = muggle_log_file_time_rot_handler_init(
				&rot_file_handler, filepath, MUGGLE_LOG_TIME_ROTATE_UNIT_DAY, 1,
				false);
			if (ret != 0) {
				fprintf(stderr,
						"failed init file rotate handler with path: %s\n",
						filepath);
				return false;
			}
			muggle_log_handler_set_level(
				(muggle_log_handler_t *)&rot_file_handler, level_file);
			muggle_log_handler_set_fmt(
				(muggle_log_handler_t *)&rot_file_handler, &formatter);

			logger->add_handler(logger,
								(muggle_log_handler_t *)&rot_file_handler);
		} else {
			static muggle_log_file_handler_t file_handler;
			int ret =
				muggle_log_file_handler_init(&file_handler, filepath, "a");
			if (ret != 0) {
				fprintf(stderr, "failed init file handler with path: %s\n",
						filepath);
			}

			muggle_log_handler_set_level((muggle_log_handler_t *)&file_handler,
										 level_file);
			muggle_log_handler_set_fmt((muggle_log_handler_t *)&file_handler,
									   &formatter);
			logger->add_handler(logger, (muggle_log_handler_t *)&file_handler);
		}
	}

	if (fn != NULL) {
		haclog_context_set_before_run_cb(fn);
	}

	return true;
}

#elif FOO_USE_ASYNC_LOG

static bool async_log_add_console_handler(int level)
{
	static haclog_console_handler_t handler;
	memset(&handler, 0, sizeof(handler));
	if (haclog_console_handler_init(&handler, 1) != 0) {
		fprintf(stderr, "failed init console handler");
		return false;
	}

	haclog_handler_set_level((haclog_handler_t *)&handler, level);
	haclog_context_add_handler((haclog_handler_t *)&handler);

	return true;
}

static bool async_log_add_file_handle(int level, const char *filepath)
{
	static haclog_file_handler_t handler;
	memset(&handler, 0, sizeof(handler));
	if (haclog_file_handler_init(&handler, filepath, "a") != 0) {
		fprintf(stderr, "failed init file handler");
		return false;
	}
	haclog_handler_set_level((haclog_handler_t *)&handler, level);
	haclog_context_add_handler((haclog_handler_t *)&handler);
	return true;
}

static bool async_log_add_timerot_handle(int level, const char *filepath)
{
	static haclog_file_time_rot_handler_t handler;
	memset(&handler, 0, sizeof(handler));
	if (haclog_file_time_rotate_handler_init(
			&handler, filepath, HACLOG_TIME_ROTATE_UNIT_DAY, 1, 1) != 0) {
		fprintf(stderr, "failed init file time rotate handler");
		return false;
	}
	haclog_handler_set_level((haclog_handler_t *)&handler, level);
	haclog_context_add_handler((haclog_handler_t *)&handler);

	return true;
}

static int foo_async_log_str2level(const char *s)
{
	if (s == NULL) {
		return -1;
	}
	if (strcmp(s, "TRACE") == 0 || strcmp(s, "trace") == 0) {
		return HACLOG_LEVEL_TRACE;
	}
	if (strcmp(s, "DEBUG") == 0 || strcmp(s, "debug") == 0) {
		return HACLOG_LEVEL_DEBUG;
	}
	if (strcmp(s, "INFO") == 0 || strcmp(s, "info") == 0) {
		return HACLOG_LEVEL_INFO;
	}
	if (strcmp(s, "WARNING") == 0 || strcmp(s, "warning") == 0 ||
		strcmp(s, "WARN") == 0 || strcmp(s, "warn") == 0) {
		return HACLOG_LEVEL_WARNING;
	}
	if (strcmp(s, "ERROR") == 0 || strcmp(s, "error") == 0) {
		return HACLOG_LEVEL_ERROR;
	}
	if (strcmp(s, "FATAL") == 0 || strcmp(s, "fatal") == 0) {
		return HACLOG_LEVEL_FATAL;
	}
	return -1;
}

static bool foo_async_log_init(const char *str_console_level,
							   const char *str_file_level, const char *filepath,
							   bool use_rotate, before_log_run_callback fn)
{
	int level_console = foo_async_log_str2level(str_console_level);
	int level_file = foo_async_log_str2level(str_file_level);

	if (level_console >= 0) {
		if (!async_log_add_console_handler(level_console)) {
			fprintf(stderr, "failed add console log handler\n");
			return false;
		}
	}

	if (level_file >= 0) {
		if (use_rotate) {
			if (!async_log_add_timerot_handle(level_file, filepath)) {
				fprintf(stderr, "failed add file log handler\n");
				return false;
			}
		} else {
			if (!async_log_add_file_handle(level_file, filepath)) {
				fprintf(stderr, "failed add file log handler\n");
				return false;
			}
		}
	}

	if (fn != NULL) {
		haclog_context_set_before_run_cb(fn);
	}

	haclog_backend_run();

	return true;
}

#endif

bool foo_log_init(const char *console_level, const char *file_level,
				  const char *filepath, bool use_rotate,
				  before_log_run_callback fn)
{
#if FOO_USE_SYNC_LOG
	return foo_sync_log_init(console_level, file_level, filepath, use_rotate,
							 fn);
#elif FOO_USE_ASYNC_LOG
	return foo_async_log_init(console_level, file_level, filepath, use_rotate,
							  fn);
#endif
}

void foo_log_init_thread_ctx()
{
#if FOO_USE_ASYNC_LOG
	haclog_thread_context_init();
#endif
}

void foo_log_cleanup_thread_ctx()
{
#if FOO_USE_ASYNC_LOG
	haclog_thread_context_cleanup();
#endif
}
