/*
 * interaction_order_display.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                     Open Source Development Lab, Inc.
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 * 1 march 2002
 */

#include <stdlib.h>
#include <interaction_order_display.h>
#include <initOrderItems.h>
#include <getOrderItems.h>

int execute_order_display(struct db_context_t *dbc,
	struct order_display_t *data)
{
	char sql_cmd[512];
	long long o_bill_addr_id=0;
	long long o_ship_addr_id=0;

	PGresult *res;
	int j;

	if (initOrderItems(data->odl_data) == W_ERROR)
	{
		return W_ERROR; // but, always OK.
	}

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_ORDER_DISPLAY_selODRInfo, data->c_uname, data->c_passwd);

	/* Execute SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	if (PQntuples(res) != 0)
	{
		j = 0;
		data->c_id = atoll(PQgetvalue(res, 0, j++)); /* pgxc modif, c_id will be used for order_display_line_t */
		strcpy(data->c_fname, PQgetvalue(res, 0, j++));
		strcpy(data->c_lname, PQgetvalue(res, 0, j++));
		strcpy(data->c_phone, PQgetvalue(res, 0, j++));
		strcpy(data->c_email, PQgetvalue(res, 0, j++));
		data->o_id = atoll(PQgetvalue(res, 0, j++));
		strcpy(data->o_date, PQgetvalue(res, 0, j++));
		data->o_sub_total = strtod(PQgetvalue(res, 0, j++), NULL);
		data->o_tax = strtod(PQgetvalue(res, 0, j++), NULL);
		data->o_total = strtod(PQgetvalue(res, 0, j++), NULL);
		strcpy(data->o_ship_type, PQgetvalue(res, 0, j++));
		strcpy(data->o_ship_date, PQgetvalue(res, 0, j++));
		strcpy(data->o_status, PQgetvalue(res, 0, j++));
		o_bill_addr_id = atoll(PQgetvalue(res, 0, j++));
		o_ship_addr_id = atoll(PQgetvalue(res, 0, j++));
		strcpy(data->cx_type, PQgetvalue(res, 0, j++));
		strcpy(data->cx_auth_id, PQgetvalue(res, 0, j++));
	}

	if ( PQntuples(res) == 0 )
	{
		data->items = 0;
		return OK;
	}
	PQclear(res);

	/*
	 * after displaying refresh order table for this customer id
	 * Create SQL command
	 * This is made only if clean_order_table mode is activated.
	 * the additional parameter is put in interaction_data structure
	 */
	if (data->clean_order == 1)
	{
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_ORDER_DISPLAY_delODRInfo, data->c_id);
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

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_ORDER_DISPLAY_selADRCNT, o_bill_addr_id, data->c_id); /* pgxc modif: addition of c_id */

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
		LOG_ERROR_MESSAGE("Could not obtain data\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	strcpy(data->billing.addr_street1, PQgetvalue(res, 0, j++));
	strcpy(data->billing.addr_street2, PQgetvalue(res, 0, j++));
	strcpy(data->billing.addr_city, PQgetvalue(res, 0, j++));
	strcpy(data->billing.addr_state, PQgetvalue(res, 0, j++));
	strcpy(data->billing.addr_zip, PQgetvalue(res, 0, j++));
	strcpy(data->billing.co_name, PQgetvalue(res, 0, j++));
	PQclear(res);
	
	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));   
	sprintf(sql_cmd,STMT_ORDER_DISPLAY_selADRCNT, o_ship_addr_id, data->c_id);

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
		LOG_ERROR_MESSAGE("Could not obtain address info\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	strcpy(data->shipping.addr_street1, PQgetvalue(res, 0, j++));
	strcpy(data->shipping.addr_street2, PQgetvalue(res, 0, j++));
	strcpy(data->shipping.addr_city, PQgetvalue(res, 0, j++));
	strcpy(data->shipping.addr_state, PQgetvalue(res, 0, j++));
	strcpy(data->shipping.addr_zip, PQgetvalue(res, 0, j++));
	strcpy(data->shipping.co_name, PQgetvalue(res, 0, j++));
	PQclear(res);

	if (getOrderItems(dbc,data->odl_data,data->o_id,data->c_id,&data->items) == W_ERROR)
	{
		/* pgxc modif, transfer of c_id */
		return W_ERROR;
	}

	return OK;
}
