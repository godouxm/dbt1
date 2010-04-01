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

#include <string.h>

#include <interaction_home.h>
#include <InsertCust.h>
#include <DigSyl.h>

int InsertCust(struct db_context_t *odbcc,
               struct buy_request_t *data)
{
	
	SQLRETURN rc;
	short co_id = 0 ;         /* temporaly country ID */
	int co_addr_id=0;
	int c_id = 0 ;
	char c_uname[21];
	char sql_cmd[2048];

	int i, strmax;
	char c_uname_small[21];
	
	bzero(c_uname, sizeof(c_uname));
	bzero(c_uname_small, sizeof(c_uname_small));
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	
	/*                       */
	/* STEP 1 Get Country ID */
	/*                       */
	
	/* Create SQL Command */
	sprintf(sql_cmd,STMT_InsertCust_getCountry, data->address.co_name);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
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

	/*                            */
	/* STEP 2 Get Country Address */
	/*                            */

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_InsertCust_getAddr,
						co_id,
						data->address.addr_zip,
						data->address.addr_state,
						data->address.addr_city,
						data->address.addr_street1,
						data->address.addr_street2);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt,1,SQL_C_ULONG,&co_addr_id,0,NULL);
	rc = SQLFetch(odbcc->hstmt);
	SQLCloseCursor(odbcc->hstmt);

	if( rc == SQL_NO_DATA_FOUND ) {
		
		/* Insert Address  */
		
		//Get addr_id from sequence
		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_InsertCust_insAddr_selInsVal);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_C_LONG,&co_addr_id,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);

		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		sprintf(sql_cmd,STMT_InsertCust_insAddr,
						co_addr_id,
						data->address.addr_street1,
						data->address.addr_street2,
						data->address.addr_city,
						data->address.addr_state,
						data->address.addr_zip,
						co_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
 		SQLCloseCursor(odbcc->hstmt);
	}
	
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	/*                            */
	/* STEP 3 Insert Customer     */
	/*                            */
	
	//get c_id from custromer tables
	/* select inserted c_id */
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_InsertCust_selInsVal);
	
	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt,1,SQL_C_LONG,&data->c_id,0,NULL);
	rc = SQLFetch(odbcc->hstmt);
	if( rc != SQL_NO_DATA_FOUND ) {
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	SQLCloseCursor(odbcc->hstmt);
	
	rc = DigSyl(++(data->c_id),0,c_uname);

	/* change big char to small char. */
	strmax=strlen(c_uname);
	for (i = 0; i < strmax; i++) {
		c_uname_small[i] = (char) tolower(c_uname[i]);
	}

	 // insert data to customer tables
	/* Create SQL Command */
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
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	SQLCloseCursor(odbcc->hstmt);

	return OK;
}
