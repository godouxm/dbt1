/*
 * InsertCust.c
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
#include <ctype.h>

#include <interaction_home.h>
#include <InsertCust.h>
#include <DigSyl.h>

int InsertCust(struct db_context_t *dbc,
               struct buy_request_t *data)
{

	short co_id = 0 ;         /* temporarily country ID */
	int co_addr_id=0;
	char c_uname[21];
	char sql_cmd[2048];

	int i, j, strmax;
	char c_uname_small[21];

	bzero(c_uname, sizeof(c_uname));
	bzero(c_uname_small, sizeof(c_uname_small));
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;

	PGresult *res;

	/*                       */
	/* STEP 1 Get Country ID */
	/*                       */

	/* Create SQL Command */
	sprintf(sql_cmd,STMT_InsertCust_getCountry, data->address.co_name);

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
		LOG_ERROR_MESSAGE("Could not obtain country id\n");
		PQclear(res);
		return ERROR;
	}


	/* Get data */
	j = 0;
	co_id = atoi(PQgetvalue(res, 0, j++));
	PQclear(res);

	/* pgxc-modif - get the new customer id first */
	/* Create SQL Command */
	// get new c_id 
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_InsertCust_selInsVal);
 
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
		LOG_ERROR_MESSAGE("Could not obtain next customer id\n");
		PQclear(res);
		return ERROR;
	}

	/* Get data */
	j = 0;
	data->c_id = atoll(PQgetvalue(res, 0, j++));
	PQclear(res);

	/* */
	if (DigSyl(++(data->c_id),0,c_uname) == W_ERROR)
	{
		return W_ERROR; // but, always OK.
	}

	/* pgxc-modif 
	 * we may want to always skip this, as it will always fail...
	*/
	/* STEP 2 Get Country Address */
	/*                            */

	/* create SQL Command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_InsertCust_getAddr,
		co_id,
		data->address.addr_zip,
		data->address.addr_state,
		data->address.addr_city,
		data->address.addr_street1,
		data->address.addr_street2,
		data->c_id); /* pgxc modification */

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
		co_addr_id = atoi(PQgetvalue(res, 0, j++));
	}

	if( PQntuples(res) == 0 )
	{
		PQclear(res);

		/* Insert Address  */

		/* Create SQL Command */
		//Get addr_id from sequence
		memset( sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_InsertCust_insAddr_selInsVal);

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
			LOG_ERROR_MESSAGE("Could not obtain next address id");
			PQclear(res);
			return ERROR;
		}

		/* Get data */
		j = 0;
		co_addr_id = atoi(PQgetvalue(res, 0, j++));
		PQclear(res);

		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_InsertCust_insAddr,
			co_addr_id,
			data->address.addr_street1,
			data->address.addr_street2,
			data->address.addr_city,
			data->address.addr_state,
			data->address.addr_zip,
			co_id,
			data->c_id); 
		/* use value obtained earlier for c_id. */


		/* we cheat a little bit but the c_id is not yet defined, this part is updated when the real c_id is known */
		/*data->c_id = 0 here);*/ /* pgxc additional */

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
		PQclear(res);
	}


	/* change big char to small char. */
	strmax=strlen(c_uname);
	for (i = 0; i < strmax; i++)
	{
		c_uname_small[i] = (char) tolower(c_uname[i]);
	}

	/* Create SQL Command */
	 // insert data to customer tables
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_InsertCust_insCust,
		data->c_id ,
		c_uname,
		c_uname_small,
		data->c_fname,
		data->c_lname,
		co_addr_id,
		data->c_phone,
		data->c_email,
		data->c_discount,
		data->c_birthdate,
		data->c_data);
	
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
