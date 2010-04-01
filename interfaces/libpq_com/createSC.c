/*
 * createSC.c 
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
#include <createSC.h>

int createSC(struct db_context_t *dbc,
			struct shopping_cart_t *data)
{
	double c_discount;
	char c_fname[15+1];
	char c_lname[15+1];
	char sql_cmd[2048];

	bzero(c_fname, sizeof(c_fname));
	bzero(c_lname, sizeof(c_lname));
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 

	PGresult *res;
	int j;

	//get next sc_id from shopping_cart table
	/* select inserted sc_id */
	/* Create SQL Command */
	sprintf(sql_cmd,STMT_createSC_selInsVal);

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
		LOG_ERROR_MESSAGE("Could not obtain shopping_cart_id\n"); 
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	data->sc_id = atol(PQgetvalue(res, 0, j++));
	PQclear(res);

	//Insert data to shopping_cart table
	if (data->c_id != 0)
	{
		/* Create SQL Command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
		sprintf(sql_cmd,STMT_createSC_selCUST,data->c_id);

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
			LOG_ERROR_MESSAGE("Could not obtain customer info\n");
			PQclear(res);
			return ERROR;
		}


		/* Get data */
		j = 0;
		strcpy(c_fname, PQgetvalue(res, 0, j++));
		strcpy(c_lname, PQgetvalue(res, 0, j++));
		c_discount = strtod(PQgetvalue(res, 0, j++), NULL);
		PQclear(res);

		// Insert data to shopping cart  tables 
		/* Create SQL Command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
		sprintf(sql_cmd,STMT_createSC_insSC1,
			data->sc_id ,
			data->c_id,
			c_fname,
			c_lname,
			c_discount);

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
	else
	{
		// if data->c_id=0  , do this

		// insert tables
		/* Create SQL Command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
		sprintf(sql_cmd,STMT_createSC_insSC2,data->sc_id);/*,data->c_id);*/

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

	return OK;
}
