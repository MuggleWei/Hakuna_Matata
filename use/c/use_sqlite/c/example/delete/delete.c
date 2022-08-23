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

static int callback(void *not_use, int argc, char **argv, char **az_col_name)
{
	LOG_INFO("---------------------");
	for (int i = 0; i < argc; i++)
	{
		LOG_INFO("%s = %s", (const char *)az_col_name[i], argv[i] ? argv[i] : "NULL");
	}
	return 0;
}

void delete(sqlite3 *db)
{
	const char *sql = "delete from user where age < 35";

	char *err_msg;
	int ret = sqlite3_exec(db, sql, callback, NULL, &err_msg);
	if (ret != SQLITE_OK)
	{
		LOG_ERROR("SQL error: %s", err_msg);
		sqlite3_free(err_msg);
	}
	else
	{
		LOG_INFO("delete completed");
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

	// insert datas
	delete(db);

	// close db
	sqlite3_close(db);

	// shutdown sqlite3
	sqlite3_shutdown();

	return 0;
}

