#include <chrono>
#include "mariadb/conncpp.hpp"
#include "muggle/c/muggle_c.h"

struct sys_args {
	char host[64];
	unsigned int port;
	char user[32];
	char passwd[32];
	char plugin_dir[128];
};

enum {
	OPT_VAL_PLUGIN_DIR = 1001,
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
			{ "plugin_dir", required_argument, NULL, OPT_VAL_PLUGIN_DIR },
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
		case OPT_VAL_PLUGIN_DIR: {
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

void CreateTable(std::unique_ptr<sql::Connection> &conn)
{
	try {
		std::unique_ptr<sql::Statement> stmt(conn->createStatement());
		stmt->executeUpdate("DROP TABLE IF EXISTS users");
		stmt->executeUpdate("CREATE TABLE IF NOT EXISTS users ("
							"id INT PRIMARY KEY AUTO_INCREMENT,"
							"name VARCHAR(16)"
							") CHARACTER SET utf8mb4 COLLATE utf8mb4_bin");
	} catch (sql::SQLException &e) {
		LOG_ERROR("failed create table: %s", e.what());
	}
}

void InsertRowsAutoCommit(std::unique_ptr<sql::Connection> &conn,
						  std::list<std::string> &names)
{
	auto ts_begin = std::chrono::system_clock::now();
	try {
		bool auto_commit = conn->getAutoCommit();
		conn->setAutoCommit(true);

		std::unique_ptr<sql::PreparedStatement> stmt(
			conn->prepareStatement("INSERT INTO users (name) VALUES (?)"));
		for (std::string &name : names) {
			stmt->setString(1, name);
			stmt->executeUpdate();
		}

		conn->setAutoCommit(auto_commit);
	} catch (sql::SQLException &e) {
		LOG_ERROR("failed insert row: %s", e.what());
	}
	auto ts_end = std::chrono::system_clock::now();
	double elapsed_ms =
		std::chrono::duration<double, std::milli>(ts_end - ts_begin).count();
	LOG_DEBUG("auto commit insert %u rows use %.3f ms", names.size(),
			  elapsed_ms);
}
void InsertRowsManualCommit(std::unique_ptr<sql::Connection> &conn,
							std::list<std::string> &names, int batch_cnt)
{
	auto ts_begin = std::chrono::system_clock::now();
	try {
		bool auto_commit = conn->getAutoCommit();
		conn->setAutoCommit(false);

		int cnt = 0;
		std::unique_ptr<sql::PreparedStatement> stmt(
			conn->prepareStatement("INSERT INTO users (name) VALUES (?)"));
		for (std::string &name : names) {
			stmt->setString(1, name);
			stmt->executeUpdate();
			++cnt;

			if (cnt >= batch_cnt) {
				conn->commit();
				cnt = 0;
			}
		}

		if (cnt > 0) {
			conn->commit();
			cnt = 0;
		}

		conn->setAutoCommit(auto_commit);
	} catch (sql::SQLException &e) {
		LOG_ERROR("failed insert row: %s", e.what());
	}
	auto ts_end = std::chrono::system_clock::now();
	double elapsed_ms =
		std::chrono::duration<double, std::milli>(ts_end - ts_begin).count();
	LOG_DEBUG("manual commit insert %u rows with commit per %d row use %.3f ms",
			  names.size(), batch_cnt, elapsed_ms);
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
#if MUGGLE_PLATFORM_WINDOWS
#else
	if (args.plugin_dir[0] != '\0') {
		setenv("MARIADB_PLUGIN_DIR", args.plugin_dir, 1);
	}
#endif

	sql::Driver *driver = sql::mariadb::get_driver_instance();

	std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));

	CreateTable(conn);

	std::list<std::string> names;
	for (int i = 0; i < 20000; i++) {
		char buf[16];
		snprintf(buf, sizeof(buf), "name%d", i);
		names.push_back(buf);
	}

	// InsertRowsAutoCommit(conn, names);
	InsertRowsManualCommit(conn, names, 16);
	InsertRowsManualCommit(conn, names, 32);
	InsertRowsManualCommit(conn, names, 64);
	InsertRowsManualCommit(conn, names, 128);
	InsertRowsManualCommit(conn, names, 256);
	InsertRowsManualCommit(conn, names, 512);
	InsertRowsManualCommit(conn, names, 1024);

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
			"Usage: %s <options>\n"
			"\t-h, --host        connect to host\n"
			"\t-u, --user        user for login\n"
			"\t-p, --password    password to use when connecting to server\n"
			"\t-P, --port        port number to use for connection\n"
			"\t  , --plugin_dir  plugin directory\n"
			"",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}
