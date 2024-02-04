#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <stdlib.h>
#include <string.h>

static void extract_error(SQLHANDLE handle, SQLSMALLINT type);

int main()
{
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret; /* ODBC API return status */

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

	// prepare stmt
	ret = SQLPrepare(
		stmt, (SQLCHAR *)"INSERT INTO students (name, email) VALUES (?, ?)",
		SQL_NTS);
	if (!SQL_SUCCEEDED(ret)) {
		fprintf(stderr, "Failed to prepare\n");
		exit(EXIT_FAILURE);
	}

	// bind parameters
	SQLCHAR name[32];
	SQLLEN len_name = 0;
	SQLCHAR email[64];
	SQLLEN len_email = 0;

	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
						   sizeof(name), 0, name, sizeof(name), &len_name);
	if (!SQL_SUCCEEDED(ret)) {
		fprintf(stderr, "Failed bind parameter 'name'\n");
		exit(EXIT_FAILURE);
	}

	ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
						   sizeof(email), 0, email, sizeof(email), &len_email);
	if (!SQL_SUCCEEDED(ret)) {
		fprintf(stderr, "Failed bind parameter 'email'\n");
		exit(EXIT_FAILURE);
	}

	// execute
	strncpy((char *)name, "John", sizeof(name) - 1);
	len_name = strlen((char *)name);
	strncpy((char *)email, "John@example.com", sizeof(email) - 1);
	len_email = strlen((char *)email);

	ret = SQLExecute(stmt);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		printf("success execute\n");
	} else {
		fprintf(stderr, "failed SQLExecute\n");
		extract_error(stmt, SQL_HANDLE_STMT);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	/* free up allocated handles */
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);

	return 0;
}

void extract_error(SQLHANDLE handle, SQLSMALLINT type)
{
	SQLINTEGER i = 0;
	SQLINTEGER native;
	SQLCHAR state[7];
	SQLCHAR text[256];
	SQLSMALLINT len;
	SQLRETURN ret;

	do {
		ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
							sizeof(text), &len);
		if (SQL_SUCCEEDED(ret))
			printf("%s:%d:%d:%s\n", state, i, native, text);
	} while (ret == SQL_SUCCESS);
}
