/*
 * getSCDetail.c 
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
#include <getSCDetail.h>

int getSCDetail(struct db_context_t *dbc,
				struct shopping_cart_line_t data[],
				long long sc_id,
				long long c_id, /* pgxc modif: c_id is in shopping_cart_t but not in shopping_cart_line_t */
				short int  *sc_size)
{

	int i = 0 ;
	char sql_cmd[512];

	PGresult *res;
	int j;

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_getSCDetail, sc_id); /* pgxc modif: c_id transfered like sc_id */

	/* Execute SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}

	if (PQntuples(res) == 0)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		LOG_ERROR_MESSAGE("Could not obtain shopping cart details\n");
		PQclear(res);
		return ERROR;
	}


	/* Get data */
	for (i=0;i<20 && i < PQntuples(res);i++)
	{
		j = 0;
		data[i].scl_i_id = atoll(PQgetvalue(res, i, j++));
		strcpy(data[i].i_title, PQgetvalue(res, i, j++));
		data[i].scl_cost = strtod(PQgetvalue(res, i, j++), NULL);
		data[i].scl_srp = strtod(PQgetvalue(res, i, j++), NULL);
		strcpy(data[i].i_backing, PQgetvalue(res, i, j++));
		data[i].scl_qty = atoi(PQgetvalue(res, i, j++));
	}
	PQclear(res);
	*sc_size = i;

	return OK;
}
