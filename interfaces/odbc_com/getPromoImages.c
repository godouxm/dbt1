/*
 * getPromoImages.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 * 18 february 2002
 */

#include <interaction_home.h>
#include <getPromoImages.h>

int getPromoImages(	struct db_context_t *odbcc,
					struct promotional_processing_t *data)
{
	
	SQLRETURN rc;
	int i = 0 ;
	char sql_cmd[512];
	
	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_getPromoImages,
						data->i_id,
						data->i_id,
						data->i_id,
						data->i_id,
						data->i_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	for (i = 0;i < 5;i++) {
		SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT,&data->i_related[i],0,NULL);
		SQLBindCol(odbcc->hstmt,2,SQL_C_SBIGINT,&data->i_thumbnail[i],0,NULL);
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

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	SQLCloseCursor(odbcc->hstmt);

	return OK;
}
