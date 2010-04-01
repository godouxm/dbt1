/*
 * interaction_admin_confirm.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SCS Sumisho Computer System corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 * 1 march 2002
 * 06 July 2005
 */

#include <stdlib.h>
#include <interaction_admin_confirm.h>

int execute_admin_confirm(struct db_context_t *dbc,
	struct admin_confirm_t *data)
{
	char sql_cmd[512];

	PGresult *res;
	int j;

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd, STMT_ADMIN_CONFIRM_UPD,
		data->i_image,
		data->i_thumbnail,
		data->i_cost,
		data->i_id);

	/* Execute SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd, STMT_ADMIN_CONFIRM_RET, data->i_id);

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
		LOG_ERROR_MESSAGE("Could not confirm admin return\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	data->i_image = atoll(PQgetvalue(res, 0, j++));
	data->i_thumbnail = atoll(PQgetvalue(res, 0, j++));
	data->i_cost = strtod(PQgetvalue(res, 0, j++), NULL);
	strcpy(data->i_title, PQgetvalue(res, 0, j++));
	strcpy(data->a_fname, PQgetvalue(res, 0, j++));
	strcpy(data->a_lname, PQgetvalue(res, 0, j++));
	strcpy(data->i_subject, PQgetvalue(res, 0, j++));
	strcpy(data->i_desc, PQgetvalue(res, 0, j++));
	data->i_srp = strtod(PQgetvalue(res, 0, j++), NULL);
	strcpy(data->i_backing, PQgetvalue(res, 0, j++));
	data->i_page = atoi(PQgetvalue(res, 0, j++));
	strcpy(data->i_publisher, PQgetvalue(res, 0, j++));
	strcpy(data->i_pub_date, PQgetvalue(res, 0, j++));
	strcpy(data->i_dimensions, PQgetvalue(res, 0, j++));
	strcpy(data->i_isbn, PQgetvalue(res, 0, j++));
	PQclear(res);

	return OK;
}
