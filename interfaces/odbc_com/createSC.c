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

#include <string.h>
#include <interaction_home.h>
#include <createSC.h>

int createSC(	struct db_context_t    *odbcc,
			struct shopping_cart_t *data)
{
	SQLRETURN rc;
	double c_discount;
	char c_fname[15+1];
	char c_lname[15+1];
	char sql_cmd[2048];
	
	bzero(c_fname, sizeof(c_fname));
	bzero(c_lname, sizeof(c_lname));
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
	
	//get next sc_id from shopping_cart table
	/* select inserted sc_id */
	/* Create SQL Command */
	sprintf(sql_cmd,STMT_createSC_selInsVal);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
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
	
	//Insert data to shopping_cart table
	if (data->c_id != 0) {

		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
		sprintf(sql_cmd,STMT_createSC_selCUST,data->c_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_CHAR,  c_fname,sizeof(c_fname),NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_CHAR,  c_lname,sizeof(c_lname),NULL);
		SQLBindCol(odbcc->hstmt,3,SQL_DOUBLE,&c_discount,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);

		// Insert data to shopping cart tables
		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
		sprintf(	sql_cmd,STMT_createSC_insSC1,
				data->sc_id ,
				data->c_id,
				c_fname,
				c_lname,
				c_discount);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			 LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		 }
		SQLCloseCursor(odbcc->hstmt);

	} else {
		// if data->c_id=0  , do this

		// insert tables
		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
		sprintf(sql_cmd,STMT_createSC_insSC2,data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);
	}

	return OK;
}
