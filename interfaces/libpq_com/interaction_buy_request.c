/*
 * interaction_buy_request.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 */

#include <interaction_buy_request.h>
#include <getCustInfo.h>
#include <InsertCust.h>
#include <updateSC.h>
#include <getSCDetail.h>

int execute_buy_request(struct db_context_t *dbc,
	struct buy_request_t *data)
{
	char sql_cmd[512];
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;

	PGresult *res;

	/* check customer status */
	if (data->returning_flag == 1)
	{
		if (getCustInfo(dbc,data) == W_ERROR)
			return W_ERROR;
	}
	else
	{
		/* first time */
		if (InsertCust(dbc,data) == W_ERROR)
			return W_ERROR;

		/* update shopping cart */
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_BUYREQ_updtsc ,
			data->c_fname,
			data->c_lname,
			data->c_discount,
			data->c_id,
			data->sc_id);

		/* Execute SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);

		/* pgxc: needs an update of shopping cart line too as scl_c_id is inserted in shopping_cart_line */
		/* Create SQL command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_BUYREQ_updtscl,
			data->c_id,
			data->sc_id);

		/* Execute SQL command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);
	}

	if (updateSC(dbc,data) == W_ERROR)
		return W_ERROR;

	/* pgxc modif: c_id is needed for getSCDetail treatment */
	if (getSCDetail(dbc,&data->scl_data[0],data->sc_id,data->c_id,&data->sc_size) == W_ERROR)
		return W_ERROR;

	return OK;
}
