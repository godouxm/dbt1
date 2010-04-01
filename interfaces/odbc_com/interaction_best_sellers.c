/*
 * interaction_best_sellers.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 * 21 february 2002
 * 06 July 2005
 */

#include <string.h>
#include <interaction_best_sellers.h>
#include <getPromoImages.h>

int execute_best_sellers(	struct db_context_t *odbcc,
						struct best_sellers_t *data)
{
	SQLRETURN rc;
	int j = 0 ;
	char sql_cmd[512];

	//note: the so-called best sellers does not retrieve correct results
	//it is the same as new_product.
	//I think this function should be re-designed in the future developement
	//Hongwei Liu

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	sprintf(sql_cmd,STMT_BEST_SELLERS, data->i_subject);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	data->items = 0 ;	
	for (j = 0; j < SEARCH_RESULT_ITEMS_MAX; j++)
	{
		// i_id
		rc = SQLBindCol(odbcc->hstmt,1,SQL_C_UBIGINT,&data->results_data[j].i_id,0,NULL);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		{
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		// i_title
   		rc = SQLBindCol(odbcc->hstmt,2,SQL_C_CHAR,data->results_data[j].i_title,
			sizeof(data->results_data[j].i_title),NULL);	
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		{
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		// a_fname	
		rc = SQLBindCol(odbcc->hstmt,3,SQL_C_CHAR,data->results_data[j].a_fname,
			sizeof(data->results_data[j].a_fname),NULL);	
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		{
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		// a_lname	
		rc = SQLBindCol(odbcc->hstmt,4,SQL_C_CHAR,data->results_data[j].a_lname,
			sizeof(data->results_data[j].a_lname),NULL);	
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		{
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		
		//get Next Row
		rc = SQLFetch(odbcc->hstmt);
		
		if(rc==SQL_NO_DATA_FOUND)
			break;
		
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
	}
	data->items = j ;

	SQLCloseCursor(odbcc->hstmt);
	
	//Get Promo Images
	data->pp_data.i_id = (UDWORD) get_random_int(item_count) + 1;
	rc = getPromoImages(odbcc,&data->pp_data);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		return W_ERROR;
	}

#ifdef DEBUG2
printf("new_products rc=%d Key1=%d Key2=%s\n",rc,data->pp_data.i_id,data->i_subject);
#endif

	if (data->items < 1)
	{
		return W_ZERO_ITEMS;
	}

	return OK;
}
