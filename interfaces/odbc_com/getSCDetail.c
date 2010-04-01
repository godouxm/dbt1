/*
 * getSCDetail.c 
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
#include <getSCDetail.h>

int getSCDetail(	struct db_context_t  *odbcc,
				struct shopping_cart_line_t data[],
				long long sc_id,
				short int  *sc_size)
{
	
	SQLRETURN rc;
	int i = 0 ;
	char sql_cmd[512];
	
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_getSCDetail, sc_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	for (i=0;i<20;i++) {
		SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT,&data[i].scl_i_id, 0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_C_CHAR,&data[i].i_title,  sizeof(data[i].i_title),NULL);
		SQLBindCol(odbcc->hstmt,3,SQL_DOUBLE,&data[i].scl_cost, 0,NULL);
		SQLBindCol(odbcc->hstmt,4,SQL_DOUBLE,&data[i].scl_srp,  0,NULL);
		SQLBindCol(odbcc->hstmt,5,SQL_C_CHAR,&data[i].i_backing,sizeof(data[i].i_backing),NULL);
		SQLBindCol(odbcc->hstmt,6,SQL_C_SSHORT,&data[i].scl_qty,  sizeof(data[i].scl_qty),NULL);
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
	*sc_size = i;

	return OK;
}
