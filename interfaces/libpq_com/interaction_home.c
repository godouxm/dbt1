/*
 * interaction_home.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd.
 *
 * 18 february 2002
 */

#include <interaction_home.h>
#include <getPromoImages.h>
#include <string.h>

int execute_home(struct db_context_t *dbc, struct home_t *data)
{

	char sql_cmd[512];

	PGresult *res;
	int j;

	memset(sql_cmd, 0x00, sizeof(sql_cmd));

	/* Generate random number for Promotional Processing. */
	data->pp_data.i_id = (unsigned long) get_random_int(item_count) + 1;

	if (data->c_id == 0)
	{
		data->c_fname[0] = 0;
		data->c_lname[0] = 0;
		return OK;
	}

	/* Create SQL Command */
	sprintf(sql_cmd,STMT_HOME, data->c_id);

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
		LOG_ERROR_MESSAGE("No data found for operation\n");
		PQclear(res);
		return ERROR;
	}


	/* Get data */
	j = 0;
	strcpy(data->c_fname, PQgetvalue(res, 0, j++));
	strcpy(data->c_lname, PQgetvalue(res, 0, j++));
	PQclear(res);

	/* get Promotion Images */
	if (getPromoImages(dbc,&data->pp_data) == W_ERROR)
		return W_ERROR;

	return OK;
}
