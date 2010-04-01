/*
 * getCustInfo.c 
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

#include <interaction_home.h>
#include <getCustInfo.h>

int getCustInfo(	struct db_context_t  *odbcc,
				struct buy_request_t *data)
{
	
	SQLRETURN rc;
	char sql_cmd[512];
	   
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_getCustInfo, data->c_uname);
	
	/* Execute get Customer Infomation. */
	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT, &data->c_id                 ,0,NULL);
	SQLBindCol(odbcc->hstmt,2,SQL_C_CHAR,   data->c_passwd             ,sizeof(data->c_passwd),NULL);
	SQLBindCol(odbcc->hstmt,3,SQL_C_CHAR,   data->c_fname              ,sizeof(data->c_fname),NULL);
	SQLBindCol(odbcc->hstmt,4,SQL_C_CHAR,   data->c_lname              ,sizeof(data->c_lname),NULL);
	SQLBindCol(odbcc->hstmt,5,SQL_C_CHAR,   data->c_phone              ,sizeof(data->c_phone),NULL);
	SQLBindCol(odbcc->hstmt,6,SQL_C_CHAR,   data->c_email              ,sizeof(data->c_email),NULL);
	SQLBindCol(odbcc->hstmt,7,SQL_C_CHAR,   data->c_data               ,sizeof(data->c_data),NULL);
	SQLBindCol(odbcc->hstmt,8,SQL_C_CHAR,   data->c_birthdate          ,sizeof(data->c_birthdate),NULL);
	SQLBindCol(odbcc->hstmt,9,SQL_C_DOUBLE,&data->c_discount           ,0,NULL);
	SQLBindCol(odbcc->hstmt,10,SQL_C_CHAR,  data->address.addr_street1 ,sizeof(data->address.addr_street1),NULL);
	SQLBindCol(odbcc->hstmt,11,SQL_C_CHAR,  data->address.addr_street2 ,sizeof(data->address.addr_street2),NULL);
	SQLBindCol(odbcc->hstmt,12,SQL_C_CHAR,  data->address.addr_city    ,sizeof(data->address.addr_city),NULL);
	SQLBindCol(odbcc->hstmt,13,SQL_C_CHAR,  data->address.addr_state   ,sizeof(data->address.addr_state),NULL);
	SQLBindCol(odbcc->hstmt,14,SQL_C_CHAR,  data->address.addr_zip     ,sizeof(data->address.addr_zip),NULL);
	SQLBindCol(odbcc->hstmt,15,SQL_C_CHAR,  data->address.co_name      ,sizeof(data->address.co_name),NULL);
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
	sprintf(sql_cmd,STMT_updateCustInfo, data->c_id);

	/* Execute get Customer Infomation. */
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
