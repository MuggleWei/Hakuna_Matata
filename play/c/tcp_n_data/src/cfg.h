#ifndef EXAMPLE_TCP_N_DATA_CFG_H_
#define EXAMPLE_TCP_N_DATA_CFG_H_

#define MUGGLE_HOLD_LOG_MACRO 1
#include "muggle/c/muggle_c.h"

EXTERN_C_BEGIN

#define ROLE_SERVER "server"
#define ROLE_CLIENT "client"

// send data one by one
#define SND_METHOD_WRITE "write"
// send data with writev
#define SND_METHOD_WRITEV "writev"
// send data as one big data
#define SND_METHOD_BIG_WRITE "big_write"
// assemble data by MSS and send one by one
#define SND_METHOD_ASSEMBLE_WRITE "assemble_write"
// assemble data by MSS and writev
#define SND_METHOD_ASSEMBLE_WRITEV "assemble_writev"

typedef struct {
	char host[MUGGLE_SOCKET_ADDR_STRLEN];
	char port[8];
	char role[8];
	char method[16];
	size_t n;
	int rcvbuf;
	int sndbuf;
} cfg_t;

bool parse_args(int argc, char **argv, cfg_t *cfg);

EXTERN_C_END

#endif // !EXAMPLE_TCP_N_DATA_CFG_H_
