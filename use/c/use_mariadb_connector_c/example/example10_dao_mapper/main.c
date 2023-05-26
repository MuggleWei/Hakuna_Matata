#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mysql.h"
#include "muggle/c/muggle_c.h"
#include "mapper_user.h"

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

	exec_sql(con, "DROP TABLE IF EXISTS users");
	exec_sql(con, "CREATE TABLE users ("
				  "id INT PRIMARY KEY AUTO_INCREMENT,"
				  "name VARCHAR(64),"
				  "age INT,"
				  "update_ts DATETIME"
				  ") CHARACTER SET utf8mb4 COLLATE utf8mb4_bin");

	// set data and execute
	struct dao_user users[] = {
		{ .name = "foo", .age = 25 },
		{ .name = "bar", .age = 30 },
		{ .name = "baz", .age = 28 },
	};

	struct tm t;
	time_t ts = time(NULL);
	gmtime_r(&ts, &t);
	for (size_t i = 0; i < sizeof(users) / sizeof(users[0]); i++) {
		users[i].update_ts.year = t.tm_year + 1900;
		users[i].update_ts.month = t.tm_mon + 1;
		users[i].update_ts.day = t.tm_mday;
		users[i].update_ts.hour = t.tm_hour;
		users[i].update_ts.minute = t.tm_min;
		users[i].update_ts.second = t.tm_sec;
	}

	// insert users
	if (!user_mapper_insert(con, users, sizeof(users) / sizeof(users[0]))) {
		LOG_ERROR("failed insert users");
		exit(EXIT_FAILURE);
	}

	// query user
	size_t rowcnt = 0;
	struct dao_user *user = NULL;
	if (!user_mapper_query_by_name(con, &users[1], &user, &rowcnt)) {
		LOG_ERROR("failed query by name");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < rowcnt; i++) {
		LOG_INFO("query result: %llu, %s, %d, %d-%02d-%02dT%02d:%02d:%02d",
				 (unsigned long long)user[i].id, user[i].name, user[i].age,
				 user[i].update_ts.year, user[i].update_ts.month,
				 user[i].update_ts.day, user[i].update_ts.hour,
				 user[i].update_ts.minute, user[i].update_ts.second);
	}

	if (user) {
		free(user);
	}

	mysql_close(con);

	mysql_library_end();

	return 0;
}
