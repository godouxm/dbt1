/*
 * interaction_home.c
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
#include <string.h>

int execute_home(struct db_context_t *odbcc, struct home_t *data)
{
	
	SQLRETURN rc;
	char sql_cmd[512];
	
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	
	/* Generate random number for Promotional Processing. */
	data->pp_data.i_id = (UDWORD) get_random_int(item_count) + 1;

	if (data->c_id == 0) {
		data->c_fname[0] = 0;
		data->c_lname[0] = 0;
		return OK;
	}

	/* Create SQL Command */
	sprintf(sql_cmd,STMT_HOME, data->c_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);	
		return W_ERROR;
	}

	/* Get data */
	SQLBindCol(odbcc->hstmt,1,SQL_C_CHAR,data->c_fname,sizeof(data->c_fname),NULL);
	SQLBindCol(odbcc->hstmt,2,SQL_C_CHAR,data->c_lname,sizeof(data->c_lname),NULL);
	rc = SQLFetch(odbcc->hstmt);
	if( rc != SQL_NO_DATA_FOUND ) {
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	SQLCloseCursor(odbcc->hstmt);

	/* get Promotion Images */
	rc = getPromoImages(odbcc,&data->pp_data);

	return OK;
}
