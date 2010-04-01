/*
 * getOrderItems.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd.
 *
 */
#include <interaction_order_display.h>
#include <getOrderItems.h>
#include <string.h>

int getOrderItems(	struct db_context_t *odbcc,
				struct order_display_line_t data[],
				long long o_id,
				short int       *num_item
				){

	SQLRETURN rc;
	char sql_cmd[512];
	
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_getOrderItems, o_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	for (*num_item = 0;*num_item < SHOPPING_CART_ITEMS_MAX;(*num_item)++) {
		SQLBindCol(odbcc->hstmt,1,SQL_C_UBIGINT, &data[*num_item].ol_i_id,0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_CHAR,     data[*num_item].i_title, sizeof(data[*num_item].i_title),NULL);
		SQLBindCol(odbcc->hstmt,3,SQL_CHAR,     data[*num_item].i_publisher,sizeof(data[*num_item].i_publisher),NULL);
		SQLBindCol(odbcc->hstmt,4,SQL_DOUBLE,  &data[*num_item].i_cost,0,NULL);
		SQLBindCol(odbcc->hstmt,5,SQL_C_SSHORT,&data[*num_item].ol_qty,0,NULL);
		SQLBindCol(odbcc->hstmt,6,SQL_DOUBLE,  &data[*num_item].ol_discount,0,NULL);
		SQLBindCol(odbcc->hstmt,7,SQL_CHAR,     data[*num_item].ol_comment,sizeof(data[*num_item].ol_comment),NULL);
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

	return OK;
}
