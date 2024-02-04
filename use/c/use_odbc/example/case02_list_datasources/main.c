#include <stdio.h>
#include <sql.h>
#include <sqlext.h>

int main()
{
	SQLHENV env;
	SQLCHAR dsn[256];
	SQLCHAR desc[256];
	SQLSMALLINT dsn_ret;
	SQLSMALLINT desc_ret;
	SQLUSMALLINT direction;
	SQLRETURN ret;

	printf("odbcinst -q -s\n\n");

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

	direction = SQL_FETCH_FIRST;
	while (SQL_SUCCEEDED(ret = SQLDataSources(env, direction, dsn, sizeof(dsn),
											  &dsn_ret, desc, sizeof(desc),
											  &desc_ret))) {
		direction = SQL_FETCH_NEXT;
		printf("%s - %s\n", dsn, desc);
		if (ret == SQL_SUCCESS_WITH_INFO) {
			printf("\tdata truncation\n");
		}
	}

	return 0;
}
