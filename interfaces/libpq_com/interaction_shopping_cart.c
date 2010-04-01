/*
 * interaction_shopping_cart.c
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

#include <interaction_shopping_cart.h>
#include <createSC.h>
#include <addToSC.h>
#include <refreshSC.h>
#include <getSCSubtotal.h>
#include <getSCDetail.h>
#include <getPromoImages.h>

int execute_shopping_cart(struct db_context_t *dbc,
	struct shopping_cart_t *data)
{
	double    sc_subtotal = 0;
	char sql_cmd[512];
	memset(sql_cmd, 0x00, sizeof(sql_cmd));

	PGresult *res;
	
	/* generate random item if add_flag == FALSE and empty (i_id, qty) pair */
	if (data->sc_size == 0 && data->add_flag == FALSE)
	{
		data->i_id = (long long)get_random_int(item_count) + 1;
	}

	/* Generate random number for Promotional Processing. */
	data->pp_data.i_id = (long long)get_random_int(item_count) + 1;

	if (data->add_flag == 1 ||
		(data->add_flag == 0 && data->sc_size ==0 && data->sc_id == 0) ||
		(data->add_flag == 0 && data->sc_size !=0))
	{
		/* STEP 1 Create Shopping Cart */
		if (data->sc_id == 0)
		{
			if (createSC(dbc,data) == W_ERROR)
				return W_ERROR;
		}
		if (data->add_flag == 1 || (data->add_flag == 0 && data->sc_size == 0))
		{
			if (addToSC(dbc,data) == W_ERROR)
				return W_ERROR;
		}
		else
		{
			if (refreshSC(dbc,data) == W_ERROR)
				return W_ERROR;
		}

		if (getSCSubtotal(dbc,data->sc_id,sc_subtotal) == W_ERROR)
			return W_ERROR;

		/* Create SQL Command */
		sprintf(sql_cmd,STMT_SC_upd,sc_subtotal,data->sc_id); /* pgxc modif: addition of c_id */

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
	/* pgxc modif: transfer of c_id needed as not included in shopping_cart_line_t */
	if (getSCDetail(dbc,&data->scl_data[0],data->sc_id,data->c_id,&data->sc_size) == W_ERROR)
		return W_ERROR;

	if (getPromoImages(dbc,&data->pp_data) == W_ERROR)
		return W_ERROR;

	return OK;
}
