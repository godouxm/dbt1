/*
 * interaction_order_display.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                     Open Source Development Lab, Inc.
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 * 1 march 2002
 */

#include <interaction_order_display.h>
#include <initOrderItems.h>
#include <getOrderItems.h>

int execute_order_display(	struct db_context_t    *odbcc,
						struct order_display_t *data)
{
	
	SQLRETURN rc;
	char sql_cmd[512];
	long long o_bill_addr_id=0;
	long long o_ship_addr_id=0;
	
	initOrderItems(data->odl_data);
	
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_ORDER_DISPLAY_selODRInfo, data->c_uname, data->c_passwd);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt, 1,SQL_C_CHAR,   data->c_fname       ,sizeof(data->c_fname),NULL);
	SQLBindCol(odbcc->hstmt, 2,SQL_C_CHAR,   data->c_lname       ,sizeof(data->c_lname),NULL);
	SQLBindCol(odbcc->hstmt, 3,SQL_C_CHAR,   data->c_phone       ,sizeof(data->c_phone),NULL);
	SQLBindCol(odbcc->hstmt, 4,SQL_C_CHAR,   data->c_email       ,sizeof(data->c_email),NULL);
	SQLBindCol(odbcc->hstmt, 5,SQL_C_UBIGINT, &data->o_id           ,0,NULL);
	SQLBindCol(odbcc->hstmt, 6,SQL_C_CHAR,   data->o_date        ,sizeof(data->o_date),NULL);
	SQLBindCol(odbcc->hstmt, 7,SQL_C_DOUBLE,&data->o_sub_total   ,0,NULL);
	SQLBindCol(odbcc->hstmt, 8,SQL_C_DOUBLE,&data->o_tax         ,0,NULL);
	SQLBindCol(odbcc->hstmt, 9,SQL_C_DOUBLE,&data->o_total       ,0,NULL);
	SQLBindCol(odbcc->hstmt,10,SQL_C_CHAR,   data->o_ship_type   ,sizeof(data->o_ship_type),NULL);
	SQLBindCol(odbcc->hstmt,11,SQL_C_CHAR,   data->o_ship_date   ,sizeof(data->o_ship_date),NULL);
	SQLBindCol(odbcc->hstmt,12,SQL_C_CHAR,   data->o_status      ,sizeof(data->o_status),NULL);
	SQLBindCol(odbcc->hstmt,13,SQL_C_UBIGINT, &o_bill_addr_id      ,0,NULL);
	SQLBindCol(odbcc->hstmt,14,SQL_C_UBIGINT, &o_ship_addr_id      ,0,NULL);
	SQLBindCol(odbcc->hstmt,15,SQL_C_CHAR,   data->cx_type       ,sizeof(data->cx_type),NULL);
	SQLBindCol(odbcc->hstmt,16,SQL_C_CHAR,   data->cx_auth_id    ,sizeof(data->cx_auth_id),NULL);
	rc = SQLFetch(odbcc->hstmt);
	if ( rc == SQL_NO_DATA_FOUND ) {
		SQLCloseCursor(odbcc->hstmt);
		data->items = 0;
		return OK;
	}
	else if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	SQLCloseCursor(odbcc->hstmt);

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_ORDER_DISPLAY_selADRCNT, o_bill_addr_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);  
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt, 1,SQL_C_CHAR,data->billing.addr_street1,sizeof(data->billing.addr_street1),NULL);
	SQLBindCol(odbcc->hstmt, 2,SQL_C_CHAR,data->billing.addr_street2,sizeof(data->billing.addr_street2),NULL);
	SQLBindCol(odbcc->hstmt, 3,SQL_C_CHAR,data->billing.addr_city   ,sizeof(data->billing.addr_city),NULL);
	SQLBindCol(odbcc->hstmt, 4,SQL_C_CHAR,data->billing.addr_state  ,sizeof(data->billing.addr_state),NULL);
	SQLBindCol(odbcc->hstmt, 5,SQL_C_CHAR,data->billing.addr_zip    ,sizeof(data->billing.addr_zip),NULL);
	SQLBindCol(odbcc->hstmt, 6,SQL_C_CHAR,data->billing.co_name     ,sizeof(data->billing.co_name),NULL);
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
	sprintf(sql_cmd,STMT_ORDER_DISPLAY_selADRCNT, o_ship_addr_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt, 1,SQL_C_CHAR,data->shipping.addr_street1,sizeof(data->shipping.addr_street1),NULL);
	SQLBindCol(odbcc->hstmt, 2,SQL_C_CHAR,data->shipping.addr_street2,sizeof(data->shipping.addr_street2),NULL);
	SQLBindCol(odbcc->hstmt, 3,SQL_C_CHAR,data->shipping.addr_city   ,sizeof(data->shipping.addr_city),NULL);
	SQLBindCol(odbcc->hstmt, 4,SQL_C_CHAR,data->shipping.addr_state  ,sizeof(data->shipping.addr_state),NULL);
	SQLBindCol(odbcc->hstmt, 5,SQL_C_CHAR,data->shipping.addr_zip    ,sizeof(data->shipping.addr_zip),NULL);
	SQLBindCol(odbcc->hstmt, 6,SQL_C_CHAR,data->shipping.co_name     ,sizeof(data->shipping.co_name),NULL);
	rc = SQLFetch(odbcc->hstmt);
	if( rc != SQL_NO_DATA_FOUND ) {
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	SQLCloseCursor(odbcc->hstmt);

	rc = getOrderItems(odbcc,data->odl_data,data->o_id,&data->items);
	if (rc == W_ERROR) {
		return W_ERROR;
	}

	return OK;
}
