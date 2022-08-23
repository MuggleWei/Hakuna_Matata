#include "muggle/c/muggle_c.h"
#include "sqlite3.h"

int init_sqlite()
{
	int ret = 0;
	ret = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
	if (ret != SQLITE_OK)
	{
		LOG_ERROR("failed set sqlite3 config: SQLITE_CONFIG_MULTITHREAD");
		return ret;
	}
	LOG_INFO("success set sqlite3 Multi-thread");

	ret = sqlite3_initialize();
	if (ret != SQLITE_OK)
	{
		LOG_ERROR("failed initialize sqlite3");
		return ret;
	}
	LOG_INFO("success initialize sqlite3");

	return 0;
}

static int callback(void *not_used, int argc, char **argv, char **az_col_name)
{
	for (int i = 0; i < argc; i++)
	{
		LOG_INFO("%s = %s", az_col_name[i], argv[i] ? argv[i] : "NULL");
	}
	return 0;
}

void create_table(sqlite3 *db)
{
	const char *sql =
		"CREATE TABLE user ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
		"name text NOT NULL, "
		"age int NOT NULL, "
		"addr char(64));";

	char *err_msg;
	int ret = sqlite3_exec(db, sql, callback, 0, &err_msg);
	if (ret != SQLITE_OK)
	{
		LOG_ERROR("SQL error: %s", err_msg);
		sqlite3_free(err_msg);
	}
	else
	{
		LOG_INFO("success create table");
	}
}

int main(int argc, char *argv[])
{
	// init log
	muggle_log_complicated_init(LOG_LEVEL_DEBUG, -1, NULL);

	// init sqlite3
	init_sqlite();

	// open database
	const char *db_name = "hello.db";

	sqlite3 *db;
	int ret = sqlite3_open(db_name, &db);
	if (ret != 0)
	{
		LOG_ERROR("failed open database: %s", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	else
	{
		LOG_INFO("success open database: %s", db_name);
	}

	// create table
	create_table(db);

	// close db
	sqlite3_close(db);

	// shutdown sqlite3
	sqlite3_shutdown();

	return 0;
}

