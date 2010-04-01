/*
 * getCustInfo.c 
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
#include <getCustInfo.h>

int getCustInfo(struct db_context_t  *dbc,
				struct buy_request_t *data)
{

	char sql_cmd[512];

	PGresult *res;
	int j;
   
	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_getCustInfo, data->c_uname);

	/* Execute get Customer Infomation. */
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
		LOG_ERROR_MESSAGE("Could not fetch customer info for c_uname: %s\n", data->c_uname);
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	strcpy(data->c_passwd, PQgetvalue(res, 0, j++));
	strcpy(data->c_fname, PQgetvalue(res, 0, j++));
	strcpy(data->c_lname, PQgetvalue(res, 0, j++));
	strcpy(data->c_phone, PQgetvalue(res, 0, j++));
	strcpy(data->c_email, PQgetvalue(res, 0, j++));
	strcpy(data->c_data, PQgetvalue(res, 0, j++));
	strcpy(data->c_birthdate, PQgetvalue(res, 0, j++));
	data->c_discount = strtod(PQgetvalue(res, 0, j++), NULL);
	strcpy(data->address.addr_street1, PQgetvalue(res, 0, j++));
	strcpy(data->address.addr_street2, PQgetvalue(res, 0, j++));
	strcpy(data->address.addr_city, PQgetvalue(res, 0, j++));
	strcpy(data->address.addr_state, PQgetvalue(res, 0, j++));
	strcpy(data->address.addr_zip, PQgetvalue(res, 0, j++));
	strcpy(data->address.co_name, PQgetvalue(res, 0, j++));
	PQclear(res);

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_updateCustInfo, data->c_id);

	/* Execute SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	return OK;
}
