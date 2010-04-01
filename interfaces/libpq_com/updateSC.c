/*
 * updateSC.c
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
#include <string.h>

#include <interaction_home.h>
#include <updateSC.h>

int updateSC(struct db_context_t *dbc,
	struct buy_request_t *data)
{
	long long   i_id[SHOPPING_CART_ITEMS_MAX];
	int         scl_qty = 0 ;
	int         sc_qty = 0;
	int         ix =0 , iy = 0 ;   /* Loop Counter */
	double i_cost[SHOPPING_CART_ITEMS_MAX];
	double sub_total = 0.0;
	char sql_cmd[512];

	PGresult *res;
	int j;
	
	for ( ix =0 ; ix < SHOPPING_CART_ITEMS_MAX ; ix++ )
	{
		i_id[ix] = 0;
		i_cost[ix] = 0;
	}
	ix = 0;

	/*                         */
	/* STEP 1 SELECT CART Info */
	/*                         */
	
	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_updateSC_sel, data->sc_id);

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
		LOG_ERROR_MESSAGE("Could not obtain shopping_cart info\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	for (ix=0;ix<SHOPPING_CART_ITEMS_MAX && ix < PQntuples(res);ix++)
	{
		j = 0;
		i_id[ix] = atoll(PQgetvalue(res, ix, j++));
		scl_qty = atoi(PQgetvalue(res, ix, j++));
		i_cost[ix] = strtod(PQgetvalue(res, ix, j++), NULL);

		sub_total += i_cost[ix]*scl_qty;
		sc_qty += scl_qty;
	}
	PQclear(res);

	/* UPDATE Shopping Cart Line */
	for (iy=0;iy<ix;iy++)
	{
		/* Create SQL Command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_updateSC_updscl,i_cost[iy],i_id[iy],data->sc_id);

		/* Execute SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);
	}

	/*                              */
	/* STEP 2 Update Shopping Cart  */
	/*                              */

	data->sc_sub_total = sub_total * (1-data->c_discount);
	data->sc_tax = data->sc_sub_total * 0.0825;
	data->sc_ship_cost = 3.00  +(1.00*sc_qty);
	data->sc_total = data->sc_sub_total + data->sc_ship_cost + data->sc_tax;

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,
		STMT_updateSC_updsc,
		data->sc_sub_total,
		data->sc_tax,
		data->sc_ship_cost,
		data->sc_total,
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

	return OK;
}
