/*
 * interaction_admin_confirm.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SCS Sumisho Computer System corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 *
 * 1 march 2002
 * 06 July 2005
 */

#include <interaction_admin_confirm.h>

int execute_admin_confirm(struct db_context_t *odbcc,
	struct admin_confirm_t *data)
{
	SQLRETURN rc;
	int j = 0 ;
	char sql_cmd[512];

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;	
	sprintf(	sql_cmd, STMT_ADMIN_CONFIRM_UPD, 
			data->i_image,
			data->i_thumbnail,
			data->i_cost,
			data->i_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	SQLCloseCursor(odbcc->hstmt);

	/* Create SQL Command */
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;	
	sprintf(sql_cmd, STMT_ADMIN_CONFIRM_RET, data->i_id);

	/* Execute SQL Command */
	rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	/* Get data */
	j = 1;
	
	// i_image
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_UBIGINT,&data->i_image, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	//i_thumbnail
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_UBIGINT,&data->i_thumbnail, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_cost
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_DOUBLE,&data->i_cost, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	// i _title
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_title, sizeof(data->i_title), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// a_fname
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->a_fname, sizeof(data->a_fname), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// a_lname
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->a_lname, sizeof(data->a_lname), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_subject
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_subject, sizeof(data->i_subject), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_desc
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_desc, sizeof(data->i_desc), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_srp
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_DOUBLE,&data->i_srp, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_backing
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_backing, 
		sizeof(data->i_backing), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_page
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_SSHORT,&data->i_page, 0, NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i _publisher
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_publisher, 
		sizeof(data->i_publisher), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_pub_date
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_pub_date, 
		sizeof(data->i_pub_date), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_dimensions
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_dimensions, 
		sizeof(data->i_dimensions), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}
	
	// i_isbn
	rc = SQLBindCol(odbcc->hstmt,j++,SQL_C_CHAR,data->i_isbn, 
		sizeof(data->i_isbn), NULL);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
	{
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	rc = SQLFetch(odbcc->hstmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO && rc != SQL_NO_DATA_FOUND) {
		LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);
		return W_ERROR;
	}

	SQLCloseCursor(odbcc->hstmt);

	return OK;
}
