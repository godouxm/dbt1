/*
 * addToSC.c 
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
#include <addToSC.h>

int addToSC(struct db_context_t *dbc,
			struct shopping_cart_t *data)
{
	int itemcount = 0;
	int scl_qty   = 0;
	long long int i_related = 0;
	double i_cost = 0.0;
	double i_srp  = 0.0;
	char i_title[60+1];
	char i_backing[15+1];
	char sql_cmd[512];

	bzero(i_title, sizeof(i_title));
	bzero(i_backing, sizeof(i_backing));
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;

	PGresult *res;
	int j;

	if (data->add_flag == 1)
	{
		/*                                */
		/* STEP 1 get Shopping Cart count */
		/*                                */
		
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_addToSC_selsumSCL,
			data->sc_id); /* pgxc additional */

		/* Execute SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
		{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}

		/* Get data */
		j = 0;
		if (PQntuples(res) != 0)
		{
			itemcount = atoi(PQgetvalue(res, 0, j++));
		}
		PQclear(res);

		if (itemcount < 100)
		{
			/*                                */
			/* STEP 2 get Shopping Cart count */
			/*                                */

			/* Create SQL Command */
			memset(sql_cmd, 0x00, sizeof(sql_cmd));
			sprintf(sql_cmd,STMT_addToSC_selSCL,data->sc_id,data->i_id);

			/* Execute SQL Command */
			res = PQexec(dbc->conn, sql_cmd);
			if (!res || PQresultStatus(res) != PGRES_TUPLES_OK )
			{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
				PQclear(res);
				return ERROR;
			}

			/* Get data */
			j = 0;
			if (PQntuples(res) != 0)
			{
				scl_qty = atoi(PQgetvalue(res, 0, j++));
			}

			if( PQntuples(res) != 0 )
			{
				PQclear(res);

				/* STEP 2.1 UPDATE Shopping Cart Line */
				/* Create SQL Command */
				memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
				sprintf(sql_cmd,STMT_addToSC_updSCL,
					++scl_qty,
					data->sc_id,
					data->i_id);

				/* Execute SQL Command */
				res = PQexec(dbc->conn, sql_cmd);
				if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
				{
					LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
					PQclear(res);
					return ERROR;
				}
				PQclear(res);

			} else {
				PQclear(res);
				
				/* STEP 2.2 INSERT Shopping Cart Line */
				/* Create SQL Command */
				memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
				sprintf(sql_cmd,STMT_addToSC_selITM1,data->i_id);

				if (data->i_id == 0)
				{
					LOG_ERROR_MESSAGE("item id equal to 0, can't select item\n");
					return ERROR;
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
					LOG_ERROR_MESSAGE("Could not obtain item data\n");
					PQclear(res);
					return ERROR;
				} 

				/* Get data */
				j = 0;
				i_cost = strtod(PQgetvalue(res, 0, j++), NULL);
				i_srp = strtod(PQgetvalue(res, 0, j++), NULL);
				strcpy(i_title, PQgetvalue(res, 0, j++));
				strcpy(i_backing, PQgetvalue(res, 0, j++));
				PQclear(res);

				/* Create SQL Command */
				memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
				sprintf(sql_cmd,STMT_addToSC_insSCL,
					data->sc_id,
					data->i_id,
					i_cost,
					i_srp,
					i_title,
					i_backing,
					data->c_id); /* pgxc additional */

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
		}
		
	}
	else
	{

		/* Create SQL Command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd)) ; 
		sprintf(sql_cmd,STMT_addToSC_selSCLC,
			data->sc_id);

		/* Execute SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK )
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}

		if (PQntuples(res) == 0)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			LOG_ERROR_MESSAGE("Could not obtain shopping cart line data id\n");
			PQclear(res);
			return ERROR;
		}

		/* Get data */
		j = 0;
		if (PQntuples(res) != 0)
		{
			scl_qty = atoi(PQgetvalue(res, 0, j++));
		}
		PQclear(res);

		if(scl_qty == 0)
		{	
			/* Create SQL Command */
			memset(sql_cmd, 0x00, sizeof(sql_cmd)); 
			sprintf(sql_cmd,STMT_addToSC_selITM2,data->i_id);

			if (data->i_id == 0) {
				LOG_ERROR_MESSAGE("item id equal to 0, can't select item\n");
				return ERROR;
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
				LOG_ERROR_MESSAGE("Could not obtain item information id\n");
				PQclear(res);
				return ERROR;
			}

			/* Get data */
			j = 0;
			i_cost = strtod(PQgetvalue(res, 0, j++), NULL);
			i_srp = strtod(PQgetvalue(res, 0, j++), NULL);
			strcpy(i_title, PQgetvalue(res, 0, j++));
			strcpy(i_backing, PQgetvalue(res, 0, j++));
			i_related = atoll(PQgetvalue(res, 0, j++));
			PQclear(res);

			/* Create SQL Command */
			memset(sql_cmd, 0x00, sizeof(sql_cmd));
			sprintf(sql_cmd,STMT_addToSC_insSCL,
				data->sc_id,
				( long long int )i_related,
				i_cost,
				i_srp,
				i_title,
				i_backing,
				data->c_id);

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
	}

	return OK;
}
