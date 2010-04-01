/*
 * getPromoImages.c 
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

#include <stdlib.h>
#include <interaction_home.h>
#include <getPromoImages.h>

int getPromoImages(	struct db_context_t *dbc,
					struct promotional_processing_t *data)
{
	int i = 0 ;
	char sql_cmd[512];

	PGresult *res;
	int j;
	
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_getPromoImages,
		data->i_id,
		data->i_id,
		data->i_id,
		data->i_id,
		data->i_id);

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
		LOG_ERROR_MESSAGE("Could not obtain promo images\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	for (i = 0;i < 5 && i < PQntuples(res);i++)
	{
		j = 0;
		data->i_related[i] = atoll(PQgetvalue(res, i, j++));
		data->i_thumbnail[i] = atoll(PQgetvalue(res, i, j++));
	}
	PQclear(res);

	return OK;
}
