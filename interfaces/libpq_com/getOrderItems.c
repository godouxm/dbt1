/*
 * getOrderItems.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd.
 *
 */

#include <stdlib.h>
#include <interaction_order_display.h>
#include <getOrderItems.h>
#include <string.h>

int getOrderItems(struct db_context_t *dbc,
	struct order_display_line_t data[],
	long long o_id,
	long long c_id, /*pgxc additional c_id has to be inserted in order_display_t */
	short int       *num_item)
{
	char sql_cmd[512];

	PGresult *res;
	int j;

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_getOrderItems, o_id, c_id);

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
		LOG_ERROR_MESSAGE("Could not obtain order items\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	for (*num_item = 0;*num_item < SHOPPING_CART_ITEMS_MAX && *num_item < PQntuples(res);(*num_item)++)
	{
		j = 0;
		data[*num_item].ol_i_id = atoll(PQgetvalue(res, *num_item, j++));
		strcpy(data[*num_item].i_title, PQgetvalue(res, *num_item, j++));
		strcpy(data[*num_item].i_publisher, PQgetvalue(res, *num_item, j++));
		data[*num_item].i_cost = strtod(PQgetvalue(res, *num_item, j++), NULL);
		data[*num_item].ol_qty = atoi(PQgetvalue(res, *num_item, j++));
		data[*num_item].ol_discount = strtod(PQgetvalue(res, *num_item, j++), NULL);
		strcpy(data[*num_item].ol_comment, PQgetvalue(res, *num_item, j++));
	}
	PQclear(res);

	return OK;
}
