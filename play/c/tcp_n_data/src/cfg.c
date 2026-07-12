#include "cfg.h"

enum {
	OPT_VAL_RCVBUF = 1000,
	OPT_VAL_SNDBUF,
};

bool parse_args(int argc, char **argv, cfg_t *cfg)
{
	const char *str_usage =
		"Usage: %s <options>\n"
		"    -h, --help    show man info\n"
		"    -H, --host    listen/connect host\n"
		"    -P, --port    listen/connect port\n"
		"    -r, --role    server|client\n"
		"    -m, --method  send method\n"
		"        * write: send data one by one\n"
		"        * writev: send data use writev\n"
		"        * big_write: combine all data into one and send\n"
		"        * assemble_write: assemble data by MSS and send one by one\n"
		"        * assemble_writev: assemble data by MSS and writev\n"
		"    -n, --number  number of data\n"
		"      , --rcvbuf  rcvbuf size\n"
		"      , --sndbuf  sndbuf size\n"
		"";

	memset(cfg, 0, sizeof(*cfg));
	strncpy(cfg->host, "127.0.0.1", sizeof(cfg->host) - 1);
	strncpy(cfg->port, "10102", sizeof(cfg->port) - 1);
	strncpy(cfg->role, "?", sizeof(cfg->role) - 1);
	strncpy(cfg->method, "write", sizeof(cfg->method) - 1);
	cfg->n = 10000;
	cfg->rcvbuf = 512 * 1024;
	cfg->sndbuf = 512 * 1024;

	int c = 0;
	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "help", no_argument, NULL, 'h' },
			{ "host", required_argument, NULL, 'H' },
			{ "port", required_argument, NULL, 'P' },
			{ "role", required_argument, NULL, 'r' },
			{ "method", required_argument, NULL, 'm' },
			{ "number", required_argument, NULL, 'n' },
			{ "rcvbuf", required_argument, NULL, OPT_VAL_RCVBUF },
			{ "sndbuf", required_argument, NULL, OPT_VAL_SNDBUF },
			{ NULL, 0, NULL, 0 }
		};
		c = getopt_long(argc, argv, "hH:P:r:m:n:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			fprintf(stdout, "%s\n", str_usage);
			exit(EXIT_SUCCESS);
		} break;
		case 'H': {
			strncpy(cfg->host, optarg, sizeof(cfg->host) - 1);
		} break;
		case 'P': {
			strncpy(cfg->port, optarg, sizeof(cfg->port) - 1);
		} break;
		case 'r': {
			strncpy(cfg->role, optarg, sizeof(cfg->role) - 1);
		} break;
		case 'm': {
			strncpy(cfg->method, optarg, sizeof(cfg->method) - 1);
		} break;
		case 'n': {
			cfg->n = atoi(optarg);
		} break;
		case OPT_VAL_RCVBUF: {
			cfg->rcvbuf = atoi(optarg);
		} break;
		case OPT_VAL_SNDBUF: {
			cfg->sndbuf = atoi(optarg);
		} break;
		default: {
			fprintf(stderr, "unrecognized opt: %s\n", optarg);
		} break;
		}
	}

	fprintf(stdout,
			"-------- input args --------\n"
			"host: %s\n"
			"port: %s\n"
			"role: %s\n"
			"method: %s\n"
			"n: %u\n"
			"rcvbuf: %d\n"
			"sndbuf: %d\n"
			"----------------------------\n",
			cfg->host, cfg->port, cfg->role, cfg->method, (unsigned int)cfg->n,
			cfg->rcvbuf, cfg->sndbuf);

	return true;
}
