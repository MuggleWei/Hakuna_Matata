#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret; /* ODBC API return status */
	SQLSMALLINT columns; /* number of columns in result-set */
	int row = 0;

	/* Allocate an environment handle */
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	/* We want ODBC 3 support */
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	/* Allocate a connection handle */
	SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
	/* Connect to the DSN mydsn */
	/* You will need to change mydsn to one you have created and tested */
	ret = SQLDriverConnect(dbc, NULL, (SQLCHAR *)"DSN=mysql-hello;", SQL_NTS,
						   NULL, 0, NULL, SQL_DRIVER_COMPLETE);

	if (!SQL_SUCCEEDED(ret)) {
		fprintf(stderr, "Failed to connect\n");
		exit(EXIT_FAILURE);
	}
	/* Allocate a statement handle */
	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	ret = SQLPrepare(stmt, (SQLCHAR *)"SELECT * from students", SQL_NTS);
	if (!SQL_SUCCEEDED(ret)) {
		fprintf(stderr, "Failed to prepare\n");
		exit(EXIT_FAILURE);
	}

	ret = SQLExecute(stmt);
	if (!SQL_SUCCEEDED(ret)) {
		fprintf(stderr, "Failed to execute\n");
		exit(EXIT_FAILURE);
	}

	SQLNumResultCols(stmt, &columns);
	while (SQLFetch(stmt) == SQL_SUCCESS) {
		SQLUSMALLINT i;
		printf("Row %d\n", row++);

		/* Loop through the columns */
		for (i = 1; i <= columns; i++) {
			SQLLEN indicator;
			char buf[512];
			/* retrieve column data as a string */
			ret = SQLGetData(stmt, i, SQL_C_CHAR, buf, sizeof(buf), &indicator);
			if (SQL_SUCCEEDED(ret)) {
				/* Handle null columns */
				if (indicator == SQL_NULL_DATA)
					strcpy(buf, "NULL");
				printf("  Column %u : %s\n", i, buf);
			}
		}
	}

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	/* free up allocated handles */
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);

	return 0;
}
