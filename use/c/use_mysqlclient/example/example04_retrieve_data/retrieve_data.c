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

void exit_with_error(MYSQL *con)
{
	LOG_ERROR("%s", mysql_error(con));
	mysql_close(con);
	exit(EXIT_FAILURE);
}

void exec_sql(MYSQL *con, const char *sqlstr)
{
	LOG_DEBUG("begin exec sql: %s", sqlstr);
	if (mysql_query(con, sqlstr)) {
		exit_with_error(con);
	}
	LOG_DEBUG("end exec sql: %s", sqlstr);
}

void process_use_result(MYSQL *con)
{
	MYSQL_RES *result = mysql_use_result(con);
	if (result == NULL) {
		exit_with_error(con);
	}

	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		for (int i = 0; i < num_fields; i++) {
			fprintf(stdout, "%s ", row[i] ? row[i] : "NULL");
		}
		fprintf(stdout, "\n");
	}

	if (mysql_errno(con) != 0) {
		LOG_ERROR("%s", mysql_error(con));
	}

	mysql_free_result(result);
}

void process_store_result(MYSQL *con)
{
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL) {
		exit_with_error(con);
	}

	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		for (int i = 0; i < num_fields; i++) {
			fprintf(stdout, "%s ", row[i] ? row[i] : "NULL");
		}
		fprintf(stdout, "\n");
	}

	mysql_free_result(result);
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

	if (mysql_real_connect(con, args.host, args.user, args.passwd, "testdb",
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

	exec_sql(con, "SELECT * FROM cars");
	LOG_INFO("do store result");
	process_store_result(con);

	exec_sql(con, "SELECT * FROM cars");
	LOG_INFO("do use result");
	process_use_result(con);

	mysql_close(con);

	mysql_library_end();

	return 0;
}

