#include <bits/chrono.h>
#include <chrono>
#include "mariadb/conncpp.hpp"
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

void Query(std::unique_ptr<sql::Connection> &conn, int fetch_row)
{
	std::unique_ptr<sql::Statement> stmt(conn->createStatement());

	if (fetch_row != 0) {
		stmt->setFetchSize(fetch_row);
	}

	auto ts_begin = std::chrono::system_clock::now();

	std::unique_ptr<sql::ResultSet> res(
		stmt->executeQuery("SELECT id, name from users"));

	int cnt = 0;
	while (res->next()) {
		if (fetch_row > 0 && cnt % fetch_row == 0) {
			LOG_TRACE("breakpoint");
		}
		LOG_TRACE("id=%u, name=%s", res->getInt(1), res->getString(2).c_str());
		++cnt;
	}

	auto ts_end = std::chrono::system_clock::now();
	double elapsed_ms =
		std::chrono::duration<double, std::milli>(ts_end - ts_begin).count();

	LOG_INFO("total rows: %d", cnt);
	LOG_INFO("resultset type: %d", stmt->getResultSetType());
	LOG_INFO("fetch size: %d", stmt->getFetchSize());
	LOG_INFO("elapsed %.3f ms", elapsed_ms);
}

void run(struct sys_args &args)
{
	const char *db_name = "testdb";

	char url[1024];
	memset(url, 0, sizeof(url));
	snprintf(url, sizeof(url), "jdbc:mariadb://%s:%u/%s", args.host, args.port,
			 db_name);

	sql::Properties properties({
		{ "user", args.user },
		{ "password", args.passwd },
		{ "autocommit", "true" },
		{ "useTls", "false" },
		{ "useCharacterEncoding", "utf8mb4" },
	});

	sql::Driver *driver = sql::mariadb::get_driver_instance();

	std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));

	LOG_INFO("----------------");
	LOG_INFO("Query");
	Query(conn, 0);

	LOG_INFO("----------------");
	LOG_INFO("Query with ResultSet streaming");
	Query(conn, 512);

	conn->close();
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	struct sys_args args;
	parse_args(argc, argv, &args);

	try {
		run(args);
	} catch (sql::SQLException &e) {
		LOG_ERROR("Error SQLException: %s", e.what());
		LOG_ERROR(
			"\n"
			"Usage: %s -h <host> -u <user> -p <password> -P <port>\n"
			"\t-h, --host      connect to host\n"
			"\t-u, --user      user for login\n"
			"\t-p, --password  password to use when connecting to server\n"
			"\t-P, --port      port number to use for connection\n"
			"",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}
