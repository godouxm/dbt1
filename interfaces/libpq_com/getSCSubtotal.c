/*
 * getSCSubtotal.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 */

#include <stdlib.h>
#include <interaction_home.h>
#include <getSCSubtotal.h>

int getSCSubtotal(struct db_context_t  *dbc,
				long long sc_id,double sc_subtotal)
{
	char sql_cmd[512];

	PGresult *res;
	int j;

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_getSCSubtotal, sc_id);

	/* Execute SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}

	if (PQntuples(res) == 0) {
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		LOG_ERROR_MESSAGE("Could not obtain shopping_cart subtotal\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	sc_subtotal = strtod(PQgetvalue(res, 0, j++), NULL);
	PQclear(res);

	return OK;
}
