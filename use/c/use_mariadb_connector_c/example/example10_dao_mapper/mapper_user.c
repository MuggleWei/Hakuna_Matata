#include "mapper_user.h"
#include "muggle/c/muggle_c.h"

#define PREPARE_STMT(con, stmt, sqlstr)                                     \
	MYSQL_STMT *stmt = mysql_stmt_init(con);                                \
	if (!stmt) {                                                            \
		LOG_ERROR("failed mysql_stmt_init");                                \
		return false;                                                       \
	}                                                                       \
	if (mysql_stmt_prepare(stmt, sqlstr, strlen(sqlstr))) {                 \
		LOG_ERROR("failed mysql_stmt_prepare: %s", mysql_stmt_error(stmt)); \
		mysql_stmt_close(stmt);                                             \
		return false;                                                       \
	}

#define CLOSE_STMT(stmt) mysql_stmt_close(stmt)

bool user_mapper_insert(MYSQL *con, struct dao_user *users, size_t rowcnt)
{
	const char *sqlstr =
		"INSERT INTO users (name, age, update_ts) VALUES (?,?,?)";

	PREPARE_STMT(con, stmt, sqlstr);

	// bind data
	char name[sizeof(users[0].name)];
	unsigned long name_len = 0;
	int age = 0;
	MYSQL_TIME ts;

	// bind
	MYSQL_BIND bind[3];
	memset(bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = name;
	bind[0].buffer_length = sizeof(name);
	bind[0].is_null = 0;
	bind[0].length = &name_len;

	bind[1].buffer_type = MYSQL_TYPE_LONG;
	bind[1].buffer = &age;
	bind[1].is_null = 0;
	bind[1].length = 0;

	bind[2].buffer_type = MYSQL_TYPE_TIMESTAMP;
	bind[2].buffer = &ts;
	bind[2].is_null = 0;
	bind[2].length = 0;

	if (mysql_stmt_bind_param(stmt, bind)) {
		LOG_ERROR("failed mysql_stmt_bind_param: %s", mysql_stmt_error(stmt));
		CLOSE_STMT(stmt);
		return false;
	}

	// insert
	bool ret = true;
	for (size_t i = 0; i < rowcnt; i++) {
		name_len = strlen(users[i].name);
		if (name_len >= sizeof(name)) {
			name_len = sizeof(name) - 1;
		}
		memcpy(name, users[i].name, name_len);
		name[name_len] = '\0';

		age = users[i].age;

		memcpy(&ts, &users[i].update_ts, sizeof(ts));

		LOG_DEBUG("begin exec sql: %s | (%s, %d, %d-%02d-%02dT%02d:%02d:%02d)",
				  sqlstr, name, age, ts.year, ts.month, ts.day, ts.hour,
				  ts.minute, ts.second);

		if (mysql_stmt_execute(stmt)) {
			LOG_ERROR("failed mysql_stmt_execute: %s", mysql_stmt_error(stmt));
			CLOSE_STMT(stmt);
			return false;
		}

		LOG_DEBUG("end exec sql: %s | (%s, %d, %d-%02d-%02dT%02d:%02d:%02d)",
				  sqlstr, name, age, ts.year, ts.month, ts.day, ts.hour,
				  ts.minute, ts.second);
	}

	uint64_t affected_row = mysql_stmt_affected_rows(stmt);
	LOG_DEBUG("total affected row: %llu", (unsigned long long)affected_row);

	CLOSE_STMT(stmt);

	return ret;
}

bool user_mapper_query_by_name(MYSQL *con, struct dao_user *user,
							   struct dao_user **users, size_t *rowcnt)
{
	const char *sqlstr =
		"SELECT id, name, age, update_ts from users where name = ?";

	PREPARE_STMT(con, stmt, sqlstr);

	// bind data
	char name[32];
	unsigned long name_len = 0;

	// bind
	MYSQL_BIND bind[1];
	memset(bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = name;
	bind[0].buffer_length = sizeof(name);
	bind[0].is_null = 0;
	bind[0].length = &name_len;

	if (mysql_stmt_bind_param(stmt, bind)) {
		LOG_ERROR("failed mysql_stmt_bind_param: %s", mysql_stmt_error(stmt));
		CLOSE_STMT(stmt);
		return false;
	}

	// query
	name_len = strlen(user->name);
	if (name_len >= sizeof(name)) {
		name_len = sizeof(name) - 1;
	}
	memcpy(name, user->name, name_len);
	name[name_len] = '\0';

	if (mysql_stmt_execute(stmt)) {
		LOG_ERROR("failed mysql_stmt_execute: %s", mysql_stmt_error(stmt));
		CLOSE_STMT(stmt);
		return false;
	}

	// result data
	uint64_t user_id = 0;
	char user_name[32];
	unsigned long user_name_len = 0;
	int user_age = 0;
	MYSQL_TIME user_update_ts;

	// result bind
	MYSQL_BIND result_bind[4];
	memset(result_bind, 0, sizeof(result_bind));

	result_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
	result_bind[0].buffer = &user_id;

	result_bind[1].buffer_type = MYSQL_TYPE_STRING;
	result_bind[1].buffer = user_name;
	result_bind[1].buffer_length = sizeof(user_name);
	result_bind[1].length = &user_name_len;

	result_bind[2].buffer_type = MYSQL_TYPE_LONG;
	result_bind[2].buffer = &user_age;

	result_bind[3].buffer_type = MYSQL_TYPE_TIMESTAMP;
	result_bind[3].buffer = &user_update_ts;

	// bind result buffers
	if (mysql_stmt_bind_result(stmt, result_bind)) {
		LOG_ERROR("failed mysql_stmt_bind_result: %s", mysql_stmt_error(stmt));
		CLOSE_STMT(stmt);
		return false;
	}

	// store result
	if (mysql_stmt_store_result(stmt)) {
		LOG_ERROR("failed mysql_stmt_store_result: %s", mysql_stmt_error(stmt));
		CLOSE_STMT(stmt);
		return false;
	}
	*rowcnt = mysql_stmt_num_rows(stmt);

	if (*rowcnt > 0) {
		*users = (struct dao_user *)malloc(sizeof(struct dao_user) * *rowcnt);
		if (*users == NULL) {
			LOG_ERROR("failed allocate memory space for users");
			CLOSE_STMT(stmt);
			return false;
		}

		// fetch all rows
		uint64_t row = 0;
		while (!mysql_stmt_fetch(stmt)) {
			(*users)[row].id = user_id;

			if (user_name_len >= sizeof((*users)[0].name)) {
				user_name_len = sizeof((*users)[0].name) - 1;
			}
			memcpy((*users)[row].name, user_name, user_name_len);
			(*users)[row].name[user_name_len] = '\0';

			(*users)[row].age = user_age;

			memcpy(&(*users)[row].update_ts, &user_update_ts,
				   sizeof(user_update_ts));

			++row;
		}
	}

	CLOSE_STMT(stmt);

	return users;
}
