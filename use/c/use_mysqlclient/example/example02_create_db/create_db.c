#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mysql.h"
#include "muggle/c/muggle_c.h"

struct sys_args {
	char host[64];
	unsigned int port;
	char user[32];
	char passwd[32];
};

bool parse_args(int argc, char **argv, struct sys_args *args)
{
	memset(args, 0, sizeof(*args));

	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "host", required_argument, NULL, 'h' },
			{ "user", required_argument, NULL, 'u' },
			{ "password", required_argument, NULL, 'p' },
			{ "port", required_argument, NULL, 'P' },
		};

		c = getopt_long(argc, argv, "h:u:p:P:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h': {
			strncpy(args->host, optarg, sizeof(args->host) - 1);
		} break;
		case 'u': {
			strncpy(args->user, optarg, sizeof(args->user) - 1);
		} break;
		case 'p': {
			strncpy(args->passwd, optarg, sizeof(args->passwd) - 1);
		} break;
		case 'P': {
			muggle_str_tou(optarg, &args->port, 10);
		} break;
		}
	}

	// default value
	if (args->host[0] == '\0') {
		strncpy(args->host, "127.0.0.1", sizeof(args->host));
	}

	if (args->port == 0) {
		args->port = 3306;
	}

	return true;
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	struct sys_args args;
	parse_args(argc, argv, &args);

	mysql_library_init(0, NULL, NULL);

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) {
		LOG_ERROR("%s", mysql_error(con));
		exit(EXIT_FAILURE);
	}

	if (mysql_real_connect(con, args.host, args.user, args.passwd, NULL,
						   args.port, NULL, 0) == NULL) {
		LOG_ERROR("%s", mysql_error(con));
		mysql_close(con);

		fprintf(stderr,
				"Usage: %s -h <host> -u <user> -p <password> -P <port>\n"
				"\t-h, --host      connect to host\n"
				"\t-u, --user      user for login\n"
				"\t-p, --password  password to use when connecting to server\n"
				"\t-P, --port      port number to use for connection\n"
				"",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	const char *sqlstr = "CREATE DATABASE IF NOT EXISTS testdb";
	LOG_DEBUG("exec sql: %s", sqlstr);
	if (mysql_query(con, sqlstr)) {
		LOG_ERROR("%s", mysql_error(con));
		mysql_close(con);
		exit(EXIT_FAILURE);
	}

	mysql_close(con);

	mysql_library_end();

	return 0;
}
