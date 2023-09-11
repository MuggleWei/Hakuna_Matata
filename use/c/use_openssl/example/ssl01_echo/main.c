#include "muggle/c/muggle_c.h"
#include "server.h"
#include "client.h"

#define ROLE_SERVER 1
#define ROLE_CLIENT 2

typedef struct sys_args {
	int role;
	char host[64];
	char port[16];
} sys_args_t;

bool parse_sys_args(int argc, char **argv, sys_args_t *args)
{
	char str_usage[1024];
	snprintf(str_usage, sizeof(str_usage),
			 "Usage: %s -R <server|client> -H <host> -P <port>\n"
			 "\t-h, --help  print help information\n"
			 "\t-R, --role  application role\n"
			 "\t-H, --host  listen/connect host\n"
			 "\t-P, --port  listen/connect port",
			 argv[0]);
	memset(args, 0, sizeof(*args));

	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "role", required_argument, NULL, 'R' },
		{ "host", required_argument, NULL, 'H' },
		{ "port", required_argument, NULL, 'P' },
		{ NULL, 0, NULL, 0 }
	};

	while (true) {
		int c = 0;
		int option_index = 0;
		c = getopt_long(argc, argv, "hR:H:P:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h': {
			LOG_INFO("%s", str_usage);
			exit(EXIT_SUCCESS);
		} break;
		case 'R': {
			if (strcmp(optarg, "server") == 0) {
				args->role = ROLE_SERVER;
			} else if (strcmp(optarg, "client") == 0) {
				args->role = ROLE_CLIENT;
			}
		} break;
		case 'H': {
			strncpy(args->host, optarg, sizeof(args->host) - 1);
		} break;
		case 'P': {
			strncpy(args->port, optarg, sizeof(args->port) - 1);
		} break;
		}
	}

	if (args->role != ROLE_SERVER && args->role != ROLE_CLIENT) {
		LOG_ERROR("run without 'role'");
		LOG_ERROR("%s", str_usage);
		return false;
	}
	if (args->host[0] == '\0') {
		LOG_WARNING("run without 'host', set 127.0.0.1 by default");
		strncpy(args->host, "127.0.0.1", sizeof(args->host) - 1);
	}
	if (args->port[0] == '\0') {
		LOG_WARNING("run without 'port', set 10102 by default");
		strncpy(args->port, "10102", sizeof(args->port) - 1);
	}

	LOG_INFO("\n"
			 "----- input args -----\n"
			 "role=%s\n"
			 "host=%s\n"
			 "port=%s\n"
			 "----------------------",
			 args->role == ROLE_SERVER ? "server" : "client", args->host,
			 args->port);

	return true;
}

int main(int argc, char *argv[])
{
	if (muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL)) {
		fprintf(stderr, "failed initialize log");
		exit(EXIT_FAILURE);
	}

	if (muggle_socket_lib_init() != 0) {
		LOG_ERROR("failed initialize socket library");
		exit(EXIT_FAILURE);
	}

	sys_args_t args;
	if (!parse_sys_args(argc, argv, &args)) {
		LOG_ERROR("failed parse input arguments");
		exit(EXIT_FAILURE);
	}

	bool ret = true;
	switch (args.role) {
	case ROLE_SERVER: {
		ret = run_ssl_server(args.host, args.port);
	} break;
	case ROLE_CLIENT: {
		ret = run_ssl_client(args.host, args.port);
	} break;
	default: {
		LOG_FATAL("invalid application role");
		exit(EXIT_FAILURE);
	} break;
	}

	if (!ret) {
		exit(EXIT_FAILURE);
	}

	return 0;
}
