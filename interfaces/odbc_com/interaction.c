/*
 * interaction.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 18 february 2002
 */

#include <string.h>
#include <pthread.h>

#include <interaction.h>

SQLHENV henv = SQL_NULL_HENV;
pthread_mutex_t db_source_mutex = PTHREAD_MUTEX_INITIALIZER;

SQLCHAR servername[32];
SQLCHAR username[32];
SQLCHAR authentication[32];

/* Open an ODBC connection to the database. */
int _db_connect(struct db_context_t *odbcc)
{
	SQLRETURN rc;
	
	/* Allocate connection handles. */
	pthread_mutex_lock(&db_source_mutex);
	rc = SQLAllocHandle(SQL_HANDLE_DBC, henv, &odbcc->hdbc);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_DBC, odbcc->hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, odbcc->hdbc);
		return W_ERROR;
	}

	/* Open connection to the database. */
	rc = SQLConnect(odbcc->hdbc, servername, SQL_NTS,
		username, SQL_NTS, authentication, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_DBC, odbcc->hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, odbcc->hdbc);
		return W_ERROR;
	}

	rc = SQLSetConnectAttr(odbcc->hdbc, SQL_ATTR_AUTOCOMMIT,
		SQL_AUTOCOMMIT_OFF, 0);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		return W_ERROR;
	}

	/* allocate statement handle */
	rc = SQLAllocHandle(SQL_HANDLE_STMT, odbcc->hdbc, &odbcc->hstmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
	        LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
	        return W_ERROR;
	}
	pthread_mutex_unlock(&db_source_mutex);

	return OK;
}

/*
 * Disconnect from the database and free the connection handle.
 * Note that we create the environment handle in odbc_connect() but
 * we don't touch it here.
 */
int _db_disconnect(struct db_context_t *odbcc)
{
	SQLRETURN rc;
	
	pthread_mutex_lock(&db_source_mutex);
	rc = SQLFreeHandle(SQL_HANDLE_STMT, odbcc->hstmt);
	if (rc != SQL_SUCCESS)
	{
	        LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
	        return W_ERROR;
	}
	rc = SQLDisconnect(odbcc->hdbc);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_DBC, odbcc->hdbc);
		return W_ERROR;
	}
	rc = SQLFreeHandle(SQL_HANDLE_DBC, odbcc->hdbc);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_DBC, odbcc->hdbc);
		return W_ERROR;
	}
	pthread_mutex_unlock(&db_source_mutex);
	return OK;
}

/* Print out all errors messages generated to the error log file. */
int odbc_error(char *filename, int line, SQLSMALLINT handle_type,
	SQLHANDLE handle)
{
	SQLCHAR sqlstate[5];
	SQLCHAR message[256];
	SQLSMALLINT i;
	char msg[1024];
	
	i = 1;
	while (SQLGetDiagRec(handle_type, handle, i, sqlstate,
		NULL, message, sizeof(message), NULL) == SQL_SUCCESS)
	{
		sprintf(msg, "[%d] sqlstate %s : %s", i, sqlstate, message);
		log_error_message(filename, line, msg);
		++i;
	}
	return OK;
}

/* Initialize ODBC environment handle and the database connect string. */
int _db_init(char *sname, char *uname, char *auth)
{
	SQLRETURN rc;

	/* Initialized the environment handle. */
	rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ERROR_MESSAGE("alloc env handle failed");
		return W_ERROR;
	}
	
	rc = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
		printf("set env failed");
		return 0;
	}

	/* Set the database connect string, username and password. */
	strcpy(servername, sname);
	strcpy(username, uname);
	strcpy(authentication, auth);
	
	return OK;
}

/* rollback and commit, copied for dbt2 */
int rollback_transaction(struct db_context_t *dbc)
{
	
	int i;
	
	i = SQLEndTran(SQL_HANDLE_DBC, dbc->hdbc, SQL_ROLLBACK);
	if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, dbc->hstmt);
		return ERROR;
	}
	
	return OK;
}

int commit_transaction(struct db_context_t *dbc)
{
	int i;
	
	i = SQLEndTran(SQL_HANDLE_DBC, dbc->hdbc, SQL_COMMIT);
	if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, dbc->hstmt);
		return ERROR;
	}
	
	return OK;
}
