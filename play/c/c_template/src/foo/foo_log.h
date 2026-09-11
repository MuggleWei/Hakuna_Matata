#ifndef MUGGLE_FOO_LOG_H_
#define MUGGLE_FOO_LOG_H_

#include "foo/config.h"

EXTERN_C_BEGIN

typedef void (*before_log_run_callback)();

bool foo_log_init(const char *console_level, const char *file_level,
				  const char *filepath, bool use_rotate,
				  before_log_run_callback fn);

void foo_log_init_thread_ctx();

void foo_log_cleanup_thread_ctx();

EXTERN_C_END

#endif // !MUGGLE_FOO_LOG_H_
