/*
 * interaction_search_results.c
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
 * 26 february 2002
 */

#include <interaction_search_results.h>
#include <getPromoImages.h>

int execute_search_results(	struct db_context_t *odbcc,
						struct search_results_t *data)
{
	
	SQLRETURN rc;
	int i = 0 ;
	char sql_cmd[512];
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	
	/* get promoimages */
	data->pp_data.i_id = (UDWORD) get_random_int(item_count) + 1;
	rc = getPromoImages(odbcc,&data->pp_data);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		return W_ERROR;
	}
	
	/* select of search_type */
	/* Create SQL Command */
	switch (data->search_type)
	{
		case SEARCH_AUTHOR:
			sprintf(sql_cmd,STMT_SEARCH_RESULTS_AUTHOR,data->search_string);
			break;
		case SEARCH_SUBJECT:
			sprintf(sql_cmd,STMT_SEARCH_RESULTS_SUBJECT,data->search_string);
			break;
		case SEARCH_TITLE:
			sprintf(sql_cmd,STMT_SEARCH_RESULTS_TITLE,data->search_string);
			break;
		default:
			return W_ERROR;
			break;
	}

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	for (i = 0; i < SEARCH_RESULT_ITEMS_MAX; i++) {
		
		rc = SQLBindCol(odbcc->hstmt,1,SQL_C_SBIGINT,&data->results_data[i].i_id, 0 , NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		rc = SQLBindCol(odbcc->hstmt,2,SQL_C_CHAR,data->results_data[i].i_title,sizeof(data->results_data[i].i_title), NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		rc = SQLBindCol(odbcc->hstmt,3,SQL_C_CHAR,data->results_data[i].a_fname,sizeof(data->results_data[i].a_fname), NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		rc = SQLBindCol(odbcc->hstmt,4,SQL_C_CHAR,&data->results_data[i].a_lname,sizeof(data->results_data[i].a_lname), NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		rc=SQLFetch(odbcc->hstmt);
		
		if(rc==SQL_NO_DATA_FOUND)
			break;
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	data->items = i ;

	SQLCloseCursor(odbcc->hstmt);

	return OK;
}
