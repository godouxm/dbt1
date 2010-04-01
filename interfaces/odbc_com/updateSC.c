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

#include <string.h>

#include <interaction_home.h>
#include <updateSC.h>

int updateSC(	struct db_context_t *odbcc,
			struct buy_request_t *data)
{

	SQLRETURN rc;
	long long   i_id[SHOPPING_CART_ITEMS_MAX];
	int         scl_qty = 0 ;
	int         sc_qty = 0;
	int         ix =0 , iy = 0 ;   /* Loop Counter */
	double i_cost[SHOPPING_CART_ITEMS_MAX];
	double sub_total = 0.0;
	char sql_cmd[512];

	for ( ix =0 ; ix < SHOPPING_CART_ITEMS_MAX ; ix++ ) {
		i_id[ix] = 0;
		i_cost[ix] = 0;
	}
	ix = 0;

	/*                         */
	/* STEP 1 SELECT CART Info */
	/*                         */

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_updateSC_sel, data->sc_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	for (ix=0;ix<SHOPPING_CART_ITEMS_MAX;ix++) {
		
		SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT,&i_id[ix]  ,0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_INTEGER,&scl_qty   ,0,NULL);
		SQLBindCol(odbcc->hstmt,3,SQL_DOUBLE,&i_cost[ix],0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc == SQL_NO_DATA_FOUND ) {
			break;
		}
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		sub_total += i_cost[ix]*scl_qty;
		sc_qty += scl_qty;
	}
	SQLCloseCursor(odbcc->hstmt);

	/* UPDATE Shopping Cart Line */
	for (iy=0;iy<ix;iy++) {

		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_updateSC_updscl,i_cost[iy],i_id[iy],data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);
	}

	/*                              */
	/* STEP 2 Update Shopping Cart  */
	/*                              */

	data->sc_sub_total = sub_total * (1-data->c_discount);
	data->sc_tax       = data->sc_sub_total * 0.0825;
	data->sc_ship_cost = 3.00  +(1.00*sc_qty);
	data->sc_total     = data->sc_sub_total + data->sc_ship_cost + data->sc_tax;

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(	sql_cmd,
			STMT_updateSC_updsc,
			data->sc_sub_total,
			data->sc_tax,
			data->sc_ship_cost,
			data->sc_total,
			data->sc_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	SQLCloseCursor(odbcc->hstmt);

	return OK;
}
