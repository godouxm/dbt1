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

#include <stdio.h>
#include <string.h>

#include <interaction_buy_confirm.h>
#include <getSCDetail.h>

int execute_buy_confirm(	struct db_context_t *odbcc,
						struct buy_confirm_t *data)
{

	SQLRETURN rc;
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
	int   ol_id = 0;
	int i_stock = 0;
	int ix,iy;        /* Loop Counter */

	for ( ix =0 ; ix < SHOPPING_CART_ITEMS_MAX ; ix++ ) {
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
	/* Create SQL Command */
	if( data->sc_id != 0 ){
	
		// if data->sc_id not Zero, Select data from Shopping_Cart Table
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_BUYCONF_selSC,data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_DOUBLE,&data->sc_sub_total,0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_DOUBLE,&data->sc_tax,0,NULL);
		SQLBindCol(odbcc->hstmt,3,SQL_DOUBLE,&data->sc_ship_cost,0,NULL);
		SQLBindCol(odbcc->hstmt,4,SQL_DOUBLE,&data->sc_total,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);

	}else{

		// if data-> sc_id is Zero, Get Shopping_Cart iD from Shopping_Cart Table
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_BUYCONF_selInsVal);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT,&data->sc_id,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);
		
		//insert data is all Zero
		data->sc_sub_total =0 ;
		data->sc_tax = 0 ;
		data->sc_ship_cost = 0 ;
		data->sc_total =0 ;
		
	}

	/* SELECT customer Info */
	/* Create SQL command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_BUYCONF_selCS,data->c_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt,1,SQL_CHAR,c_fname,sizeof(c_fname),NULL);
	SQLBindCol(odbcc->hstmt,2,SQL_CHAR,c_lname,sizeof(c_lname),NULL);
	SQLBindCol(odbcc->hstmt,3,SQL_DOUBLE,&data->c_discount,0,NULL);
	SQLBindCol(odbcc->hstmt,4,SQL_C_SBIGINT,&c_addr_id,0,NULL);
	rc = SQLFetch(odbcc->hstmt);
	if( rc != SQL_NO_DATA_FOUND ) {
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	SQLCloseCursor(odbcc->hstmt);

	if (data->shipping.addr_street1[0] != 0x00) {
		
		/* SELECT ADDRESS  */
		/* Create SQL command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(	sql_cmd,STMT_BUYCONF_selADRCNT,
				data->shipping.co_name,
				data->shipping.addr_zip,
				data->shipping.addr_state,
				data->shipping.addr_city,
				data->shipping.addr_street1,
				data->shipping.addr_street2);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT,&ship_addr_id,0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_C_SHORT,&co_id,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);

		if( rc != SQL_NO_DATA_FOUND ) {
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				return W_ERROR;
			}
		} else {

			/* Create SQL command */
			memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
			sprintf(sql_cmd,STMT_BUYCONF_getCountry, data->shipping.co_name);

			/* Execute SQL Command */
			rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				return W_ERROR;
			}

			/* Get data */
			SQLBindCol(odbcc->hstmt,1,SQL_SMALLINT,&co_id,0,NULL);
			rc = SQLFetch(odbcc->hstmt);
			if( rc != SQL_NO_DATA_FOUND ) {
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
					LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
					return W_ERROR;
				}
			}
			SQLCloseCursor(odbcc->hstmt);
				
			//select id from sequense
			/* Create SQL command */
			memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
			sprintf(sql_cmd,STMT_BUYCONF_selADRID);

			/* Execute SQL Command */
			rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}

			/* Get data */
			SQLBindCol(odbcc->hstmt,1,SQL_C_UBIGINT,&ship_addr_id,0,NULL);
			rc = SQLFetch(odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			if( rc != SQL_NO_DATA_FOUND ) {
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
					LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
					return W_ERROR;
				}
			}	
			
			/* INSERT ADDRESS  */
			/* Create SQL command */
			memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
			sprintf(	sql_cmd,STMT_BUYCONF_insADR,
					ship_addr_id ,
					data->shipping.addr_street1,
					data->shipping.addr_street2,
					data->shipping.addr_city,
					data->shipping.addr_state,
					data->shipping.addr_zip,
					co_id);

			/* Execute SQL Command */
			rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
			SQLCloseCursor(odbcc->hstmt);
		}
		
	} else {
		
		/* SELECT ADDRESS  */
		/* Create SQL command */
		ship_addr_id = c_addr_id;
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_BUYCONF_selADR,ship_addr_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_SMALLINT,&co_id,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);
	}
	
	/* INSERT ORDERS  */
	/* Create SQL command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_BUYCONF_insODR,
			data->sc_id,
			data->c_id,
			data->sc_sub_total,
			data->sc_tax,
			data->sc_total,
			ship_day,
			c_addr_id,
 			ship_addr_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	SQLCloseCursor(odbcc->hstmt);
	
	/* SELECT Shopping Cart Line  */
	/* Create SQL command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_BUYCONF_selSCL,data->sc_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	for (ix=0;ix<SHOPPING_CART_ITEMS_MAX;ix++) {
		SQLBindCol(odbcc->hstmt,1,SQL_C_UBIGINT, &scl_i_id[ix],0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_DOUBLE,  &scl_cost[ix],0,NULL);
		SQLBindCol(odbcc->hstmt,3,SQL_C_SHORT,&scl_qty[ix], 0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc == SQL_NO_DATA_FOUND ) {
			break;
		}
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	SQLCloseCursor(odbcc->hstmt);

	for (iy=0;iy<ix;iy++) {
		
		/* INSERT ORDER Line  */
		/* Create SQL command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf( sql_cmd,STMT_BUYCONF_insODRL,
					++ol_id,
					data->sc_id,
					scl_i_id[iy],
					scl_qty[iy],
					data->c_discount,
 					ol_comment);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);

		/* SELECT Item  */
		/* Create SQL command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_BUYCONF_selITM, scl_i_id[iy]);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_C_SHORT,&co_id,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);

		if (i_stock - 10 > scl_qty[iy]) {
			i_stock -= scl_qty[iy];
		} else {
			i_stock = i_stock - scl_qty[iy] + 21;
		}

		/* UPDATE Item  */
		/* Create SQL command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_BUYCONF_updITM, i_stock,scl_i_id[iy]);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);
	}

	/* INSERT cc_xacts  */
	/* Create SQL command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_BUYCONF_insXACT,
					data->sc_id,
					data->cx_type,
					data->cx_num,
					data->cx_name,
					data->cx_expiry,
					cx_auth_id,
					data->sc_total,
					co_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	SQLCloseCursor(odbcc->hstmt);

	rc = getSCDetail(	odbcc,
					data->scl_data,
					data->sc_id,
					&data->sc_size);
	
	if (rc == W_ERROR) {
		return W_ERROR;
	}

	return OK;
}
