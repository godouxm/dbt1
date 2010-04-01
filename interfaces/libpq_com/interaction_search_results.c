/*
 * interaction_search_results.c
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
 * 26 february 2002
 */

#include <stdlib.h>
#include <interaction_search_results.h>
#include <getPromoImages.h>

int execute_search_results(struct db_context_t *dbc,
	struct search_results_t *data)
{
	int i = 0 ;
	char sql_cmd[512];
	memset(sql_cmd, 0x00, sizeof(sql_cmd));

	PGresult *res;
	int j;

	/* get promoimages */
	data->pp_data.i_id = (unsigned long) get_random_int(item_count) + 1;
	if (getPromoImages(dbc,&data->pp_data) == W_ERROR)
		return W_ERROR;

	/* select of search_type */
	/* Create SQL Command */
	switch (data->search_type)
	{
		case SEARCH_AUTHOR:
			sprintf(sql_cmd,STMT_SEARCH_RESULTS_AUTHOR,data->search_string);
			break;
		case SEARCH_SUBJECT:
			sprintf(sql_cmd,STMT_SEARCH_RESULTS_SUBJECT,data->search_string);
			break;
		case SEARCH_TITLE:
			sprintf(sql_cmd,STMT_SEARCH_RESULTS_TITLE,data->search_string);
			break;
		default:
			return W_ERROR;
			break;
	}

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
		LOG_ERROR_MESSAGE("No results for query.\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
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

	return OK;
}
