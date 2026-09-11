#ifndef MUGGLE_FOO_CONFIG_H_
#define MUGGLE_FOO_CONFIG_H_

#include "foo/config.h"

EXTERN_C_BEGIN

typedef struct {
	char host[64];
	char port[16];
	char log_console_level[8];
	char log_file_level[8];
	bool log_rotate;
} foo_config_t;

bool parse_sys_args(int argc, char **argv, foo_config_t *cfg);

EXTERN_C_END

#endif // !MUGGLE_FOO_CONFIG_H_
