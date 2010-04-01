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

pthread_mutex_t db_source_mutex = PTHREAD_MUTEX_INITIALIZER;

char sname[32];
char dbname[32];
char uname[32];
char auth[32];
char dbport[32];

int _db_connect(struct db_context_t *dbc)
{
	const char *conninfo;
	char tmp[256];
	/* Allocate connection handles. */
	pthread_mutex_lock(&db_source_mutex);

	sprintf(tmp, "host=%s dbname=%s user=%s password=%s port=%s", sname, dbname, uname, auth, dbport);
	conninfo = tmp;
	//dbc->conn = PQconnectdb("host=172.16.7.1 dbname=DBT1 user=masonsharp password=masonsharp port=5445");
	dbc->conn = PQconnectdb(conninfo);

	if ( PQstatus(dbc->conn)==CONNECTION_BAD )
	{
		LOG_ERROR_MESSAGE("Connection to database '%s' failed.",
			dbname);
		LOG_ERROR_MESSAGE("%s", PQerrorMessage(dbc->conn));
		PQfinish(dbc->conn);
		return ERROR;
	}
	printf("database connection thread_id[%d], backend_pid[%d]\n", (int)pthread_self(), PQbackendPID(dbc->conn));

	pthread_mutex_unlock(&db_source_mutex);

	return OK;
}

int
_db_disconnect(struct db_context_t *dbc)
{
	pthread_mutex_lock(&db_source_mutex);

	PQfinish(dbc->conn);

	pthread_mutex_unlock(&db_source_mutex);

	return OK;
}

int _db_init(char *_sname, char *_dbname, char *_uname, char *_auth, char *_dbport)
{
	if (_sname!= NULL)
		strcpy(sname, _sname);

	if (_dbname != NULL)
		strcpy(dbname, _dbname);

	if (_uname != NULL)
		strcpy(uname, _uname);

	if (_auth != NULL)
		strcpy(auth, _auth);

	if (_dbport != NULL)
		strcpy(dbport, _dbport);

	return OK;
}

int commit_transaction(struct db_context_t *dbc)
{
	PGresult *res;

	res = PQexec(dbc->conn, "COMMIT");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	return OK;
}

int rollback_transaction(struct db_context_t *dbc)
{
	PGresult *res;

	res = PQexec(dbc->conn, "ROLLBACK");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	return OK;
}

int begin_transaction(struct db_context_t *dbc)
{
	PGresult *res;

	res = PQexec(dbc->conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	return OK;
}

