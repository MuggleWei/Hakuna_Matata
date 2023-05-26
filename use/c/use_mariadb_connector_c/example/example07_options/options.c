#include "muggle/c/muggle_c.h"
#include "mysql.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct sys_args {
	char host[64];
	unsigned int port;
	char user[32];
	char passwd[32];
	int ssl_mode;
	char charset[16];
	char plugin_dir[MUGGLE_MAX_PATH];
};

#define OPT_SSL_MODE 1001
#define OPT_CHARSET 1002
#define OPT_PLUGIN_DIR 1003

bool parse_args(int argc, char **argv, struct sys_args *args)
{
	memset(args, 0, sizeof(*args));
	args->ssl_mode = 0;
	strncpy(args->charset, "utf8mb4", sizeof(args->charset) - 1);

	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "host", required_argument, NULL, 'h' },
			{ "user", required_argument, NULL, 'u' },
			{ "password", required_argument, NULL, 'p' },
			{ "port", required_argument, NULL, 'P' },
			{ "ssl-mode", required_argument, NULL, OPT_SSL_MODE },
			{ "charset", required_argument, NULL, OPT_CHARSET },
			{ "plugin-dir", required_argument, NULL, OPT_PLUGIN_DIR },
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
		case OPT_SSL_MODE: {
			muggle_str_toi(optarg, &args->ssl_mode, 10);
		} break;
		case OPT_CHARSET: {
			strncpy(args->charset, optarg, sizeof(args->charset) - 1);
		} break;
		case OPT_PLUGIN_DIR: {
			strncpy(args->plugin_dir, optarg, sizeof(args->plugin_dir) - 1);
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

	if (mysql_options(con, MYSQL_OPT_SSL_ENFORCE, &args.ssl_mode)) {
		LOG_ERROR("failed set ssl-mode: %d", args.ssl_mode);
	}

	if (mysql_options(con, MYSQL_SET_CHARSET_NAME, args.charset)) {
		LOG_ERROR("failed set charset: %s", args.charset);
	}

	if (args.plugin_dir[0] != '\0') {
		if (mysql_optionsv(con, MYSQL_PLUGIN_DIR, args.plugin_dir)) {
			LOG_ERROR("failed set plugin-dir: %s", args.plugin_dir);
		}
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
				"\t  , --ssl-mode  set ssl mode\n"
				"\t  , --charset   charset\n"
				"\t  , --plugin-dir plugin directory\n"
				"",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	exec_sql(con, "DROP TABLE IF EXISTS language");
	exec_sql(con, "CREATE TABLE language ("
				  "id INT PRIMARY KEY AUTO_INCREMENT,"
				  "word VARCHAR(16)"
				  ") CHARACTER SET utf8mb4 COLLATE utf8mb4_bin");
	exec_sql(con, "INSERT INTO language VALUES(1, '你好')");
	exec_sql(con, "INSERT INTO language VALUES(2, 'hello')");
	exec_sql(con, "INSERT INTO language VALUES(3, 'bonjour')");
	exec_sql(con, "INSERT INTO language VALUES(4, 'Buen día')");
	exec_sql(con, "INSERT INTO language VALUES(5, 'こんにちは')");

	mysql_close(con);

	mysql_library_end();

	return 0;
}
