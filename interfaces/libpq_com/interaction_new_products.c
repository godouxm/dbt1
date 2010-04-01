/*
 * interaction_new_products.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 * 21 february 2002
 * 06 July 2005
 */

#include <stdlib.h>
#include <string.h>
#include <interaction_new_products.h>
#include <getPromoImages.h>

int execute_new_products(struct db_context_t *dbc,
	struct new_products_t *data)
{
	int i = 0 ;
	char sql_cmd[512];

	PGresult *res;
	int j;

	/* Create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_NEW_PRODUCTS, data->i_subject);

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
		LOG_ERROR_MESSAGE("Could not obtain new product data\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	data->items = 0 ;
	for (i = 0; i < SEARCH_RESULT_ITEMS_MAX && i < PQntuples(res); i++)
	{
		j = 0;
		data->results_data[i].i_id = atoll(PQgetvalue(res, i, j++));
		strcpy(data->results_data[i].i_title, PQgetvalue(res, i, j++));
		strcpy(data->results_data[i].a_fname, PQgetvalue(res, i, j++));
		strcpy(data->results_data[i].a_lname, PQgetvalue(res, i, j++));
	}
	data->items = i;
	PQclear(res);

	data->pp_data.i_id = (unsigned long) get_random_int(item_count) + 1;
	if (getPromoImages(dbc,&data->pp_data) == W_ERROR)
		return W_ERROR;

#ifdef DEBUG2
	printf("new_products rc=%d Key1=%d Key2=%s\n",rc,data->pp_data.i_id,data->i_subject);
#endif

	if (data->items < 1)
		return W_ZERO_ITEMS;

	return OK;
}
