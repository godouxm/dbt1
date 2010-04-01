/*
 * interaction_buy_confirm.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *          Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 * 26 february 2002
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <interaction_buy_confirm.h>
#include <getSCDetail.h>

int execute_buy_confirm(struct db_context_t *dbc,
	struct buy_confirm_t *data)
{
	short int ship_day = 0;
	char ol_comment[OL_COMMENT_LEN + 1];
	char cx_auth_id[CX_AUTH_ID_LEN + 1];
	char sql_cmd[2048];
	long long  c_addr_id = 0;
	char c_fname[C_FNAME_LEN+1];
	char c_lname[C_LNAME_LEN+1];
	short co_id = 0;
	long long  ship_addr_id = 0;
	long long scl_i_id[SHOPPING_CART_ITEMS_MAX];
	short scl_qty[SHOPPING_CART_ITEMS_MAX];
	double scl_cost[SHOPPING_CART_ITEMS_MAX];
	int	ol_id = 0;
	int st_stock = 0;
	int ix,iy;        /* Loop Counter */

	PGresult *res;
	int j;

	for ( ix =0 ; ix < SHOPPING_CART_ITEMS_MAX ; ix++ )
	{
		scl_i_id[ix] = 0;
		scl_qty[ix] = 0;
		scl_cost[ix] = 0;
	}
	ix = 0;

	bzero(ol_comment,sizeof(ol_comment));
	bzero(cx_auth_id,sizeof(cx_auth_id));
	bzero(sql_cmd,sizeof(sql_cmd));
	bzero(c_fname,sizeof(c_fname));
	bzero(c_lname,sizeof(c_lname));

	/* Generate random day between 1 to 7 for ship_date. */
	ship_day = get_random_int(7) + 1;
	/* get authentication id from PGE */
	strcpy(cx_auth_id, "012345678912345");
	/* generate a_string[20..100] as order_line comment */
	get_a_string(ol_comment, 20, 100);


	/* SELECT shopping_cart Info */
	if( data->sc_id != 0 )
	{

		// if data->sc_id not Zero, Select data from Shopping_Cart Table
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_BUYCONF_selSC2,data->sc_id); 

		/* **MOBILE CODE** SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}

		if (PQntuples(res) != 0)
		{
			/* Get data */
			j = 0;
			data->sc_sub_total = strtod(PQgetvalue(res, 0, j++), NULL);
			data->sc_tax = strtod(PQgetvalue(res, 0, j++), NULL);
			data->sc_ship_cost = strtod(PQgetvalue(res, 0, j++), NULL);
			data->sc_total = strtod(PQgetvalue(res, 0, j++), NULL);
			PQclear(res);
		}
		else
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			LOG_ERROR_MESSAGE("Could not obtain shopping_cart info for sc_id = %d\n", data->sc_id); /* pgxc modif: addition of c_id */
			PQclear(res);
			return ERROR;
		}
	}
	else
	{
		// if data-> sc_id is Zero, Get Shopping_Cart iD from Shopping_Cart Table
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_BUYCONF_selInsVal);

		/* **MOBILE CODE** SQL Command */
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
			data->sc_id = atoll(PQgetvalue(res, 0, j++));
			PQclear(res);
		}
		else
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			LOG_ERROR_MESSAGE("Could not obtain shopping_cart_id\n"); /* pgxc modif: addition of c_id */
			PQclear(res);
			return ERROR;
		}
 
		//insert data is all Zero
		data->sc_sub_total = 0;
		data->sc_tax = 0;
		data->sc_ship_cost = 0;
		data->sc_total = 0;
	}

	/* SELECT customer Info */
	/* Create SQL command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_BUYCONF_selCS,data->c_id);

	/* **MOBILE CODE** SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}

	if (PQntuples(res) != 0)
	{
		/* Get data */
		j = 0;
		strcpy(c_fname, PQgetvalue(res, 0, j++));
		strcpy(c_lname, PQgetvalue(res, 0, j++));
		data->c_discount = strtod(PQgetvalue(res, 0, j++), NULL);
		c_addr_id = atoll(PQgetvalue(res, 0, j++));
		PQclear(res);
	}
	else
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		LOG_ERROR_MESSAGE("Could not select customer = %d\n", data->c_id); /* pgxc modif: addition of c_id */
		PQclear(res);
		return ERROR;
	}

	if (data->shipping.addr_street1[0] != 0x00)
	{
		/* SELECT ADDRESS  */
		/* Create SQL command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd, STMT_BUYCONF_selADRCNT,
			data->shipping.co_name,
			data->shipping.addr_zip,
			data->shipping.addr_state,
			data->shipping.addr_city,
			data->shipping.addr_street1,
			data->shipping.addr_street2,
			data->c_id); /* pgxc additional */

		/* **MOBILE CODE** SQL Command */
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
			ship_addr_id = atoll(PQgetvalue(res, 0, j++));
			co_id = atoi(PQgetvalue(res, 0, j++));
		}

		if(PQntuples(res) == 0)
		{
			PQclear(res);

			/* Create SQL command */
			memset(sql_cmd, 0x00, sizeof(sql_cmd));
			sprintf(sql_cmd,STMT_BUYCONF_getCountry, data->shipping.co_name);

			/* **MOBILE CODE** SQL Command */
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
				co_id = atoi(PQgetvalue(res, 0, j++));
			}
			else
			{
				LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			    LOG_ERROR_MESSAGE("Could not obtain country for %s", data->shipping.co_name);
			    PQclear(res);
			    return ERROR;
			}
			PQclear(res);

			//select id from sequense
			/* Create SQL command */
			memset(sql_cmd, 0x00, sizeof(sql_cmd));
			sprintf(sql_cmd,STMT_BUYCONF_selADRID);

			/* **MOBILE CODE** SQL Command */
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
				ship_addr_id = atoll(PQgetvalue(res, 0, j++));
				PQclear(res);
			}
			else
			{
				LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
				LOG_ERROR_MESSAGE("Could not obtain addr_id");
				PQclear(res);
				return ERROR;
			}

			/* INSERT ADDRESS  */
			/* Create SQL command */
			memset(sql_cmd, 0x00, sizeof(sql_cmd));
			sprintf(sql_cmd,STMT_BUYCONF_insADR,
				ship_addr_id,
				data->shipping.addr_street1,
				data->shipping.addr_street2,
				data->shipping.addr_city,
				data->shipping.addr_state,
				data->shipping.addr_zip,
				co_id,
				data->c_id);

			/* **MOBILE CODE** SQL Command */
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

	}
	else
	{

		/* SELECT ADDRESS  */
		/* Create SQL command */
		ship_addr_id = c_addr_id;
		memset(sql_cmd, 0x00, sizeof(sql_cmd));

		if (data->c_id > 0) 
			sprintf(sql_cmd,STMT_BUYCONF_selADR,ship_addr_id,data->c_id); /* pgxc modif: addition of c_id */
		else
			sprintf(sql_cmd,STMT_BUYCONF_selADR_nocust,ship_addr_id); /* pgxc modif */

		/* **MOBILE CODE** SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK )
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}

		/* Get data */
		if (PQntuples(res) != 0)
		{
			j = 0;
			co_id = atoi(PQgetvalue(res, 0, j++));
			PQclear(res);
		}
		else
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			LOG_ERROR_MESSAGE("Could not obtain address for addr_id = %d, c_id = %lld\n", ship_addr_id, data->c_id); /* pgxc modif: addition of c_id */
			PQclear(res);
			return ERROR;
		}
	}

	/* INSERT ORDERS  */
	/* Create SQL command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_BUYCONF_insODR,
		data->sc_id,
		data->c_id,
		data->sc_sub_total,
		data->sc_tax,
		data->sc_total,
		ship_day,
		c_addr_id,
		ship_addr_id);

	/* **MOBILE CODE** SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	/* seems to block here in case scl was created with a c_id null if the customer made the shopping cart before being registered */
	/* SELECT Shopping Cart Line  */
	/* Create SQL command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) );
	sprintf(sql_cmd,STMT_BUYCONF_selSCL,data->sc_id); /*,data->c_id);*/

	/* **MOBILE CODE** SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}

	if (PQntuples(res) != 0)
	{
		/* Get data */
		for (ix=0;ix<SHOPPING_CART_ITEMS_MAX && ix < PQntuples(res);ix++)
		{
			j = 0;
			scl_i_id[ix] = atoll(PQgetvalue(res, ix, j++));
			scl_cost[ix] = strtod(PQgetvalue(res, ix, j++), NULL);
			scl_qty[ix] = atoi(PQgetvalue(res, ix, j++));
		}
		PQclear(res);
	}
	else
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		LOG_ERROR_MESSAGE("Could not select shopping_cart for sc_id = %d\n", data->sc_id); /* pgxc modif: addition of c_id */
		PQclear(res);
		return ERROR;
	}

	for (iy=0;iy<ix;iy++)
	{

		/* INSERT ORDER Line  */
		/* Create SQL command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_BUYCONF_insODRL,
			++ol_id,
			data->sc_id,
			scl_i_id[iy],
			scl_qty[iy],
			data->c_discount,
			ol_comment,
			data->c_id); /* pgxc additional */

		/* **MOBILE CODE** SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);
  
		/* SELECT Item  */
		/* Create SQL command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_BUYCONF_selITM, scl_i_id[iy]); /* for pgxc nothing changed here, not necessary as stock is a partition of item just depending on it */

		/* **MOBILE CODE** SQL Command */
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
			co_id = atoi(PQgetvalue(res, 0, j++));
			PQclear(res);
		}
		else
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			LOG_ERROR_MESSAGE("Could not obtain item = %d\n", scl_i_id[iy]); /* pgxc modif: addition of c_id */
			PQclear(res);
			return ERROR;
		}

		if (st_stock - 10 > scl_qty[iy])
		{
			st_stock -= scl_qty[iy];
		}
		else
		{
			st_stock = st_stock - scl_qty[iy] + 21;
		}

		/* UPDATE Item  */
		/* Create SQL command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		sprintf(sql_cmd,STMT_BUYCONF_updITM, st_stock,scl_i_id[iy]); /* pgxc modif: name of stock variable changed to st_stock */

		/* **MOBILE CODE** SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}
	PQclear(res);
	}
 
	/* INSERT cc_xacts  */
	/* Create SQL command */
	memset(sql_cmd, 0x00, sizeof(sql_cmd));
	sprintf(sql_cmd,STMT_BUYCONF_insXACT,
		data->sc_id,
		data->cx_type,
		data->cx_num,
		data->cx_name,
		data->cx_expiry,
		cx_auth_id,
		data->sc_total,
		co_id,
		data->c_id); /* pgxc additional */

	/* **MOBILE CODE** SQL Command */
	res = PQexec(dbc->conn, sql_cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
		PQclear(res);
		return ERROR;
	}
	PQclear(res);

	if (getSCDetail(dbc, data->scl_data, data->sc_id, data->c_id, &data->sc_size) == W_ERROR)
	{
		return W_ERROR;
	}

	return OK;
}
