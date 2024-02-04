#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdio.h>

int main()
{
	SQLHENV env;
	SQLCHAR driver[256];
	SQLCHAR attr[256];
	SQLSMALLINT driver_ret;
	SQLSMALLINT attr_ret;
	SQLUSMALLINT direction;
	SQLRETURN ret;

	printf("odbcinst -q -d\n");

	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

	direction = SQL_FETCH_FIRST;
	while (SQL_SUCCEEDED(ret = SQLDrivers(env, direction, driver,
										  sizeof(driver), &driver_ret, attr,
										  sizeof(attr), &attr_ret))) {
		printf("\n");
		direction = SQL_FETCH_NEXT;
		printf("[%s]\n%s\n", driver, attr);
		if (ret == SQL_SUCCESS_WITH_INFO) {
			printf("\tdata truncation\n");
		}
	}

	return 0;
}
