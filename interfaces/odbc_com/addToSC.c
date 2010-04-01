/*
 * addToSC.c 
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
#include <addToSC.h>

int addToSC(	struct db_context_t    *odbcc,
			struct shopping_cart_t *data)
{
	SQLRETURN rc;
	int itemcount = 0;
	int scl_qty   = 0;
	SQLINTEGER i_related = 0;
	double i_cost = 0.0;
	double i_srp  = 0.0;
	char i_title[60+1];
	char i_backing[15+1];
	char sql_cmd[512];
	
	bzero(i_title, sizeof(i_title));
	bzero(i_backing, sizeof(i_backing));
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
	
	if (data->add_flag == 1) {
		/*                                */
		/* STEP 1 get Shopping Cart count */
		/*                                */
		
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_addToSC_selsumSCL,data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_INTEGER,&itemcount,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		if( rc != SQL_NO_DATA_FOUND ) {
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
		}
		SQLCloseCursor(odbcc->hstmt);
		
		if (itemcount < 100) {
			/*                                */
			/* STEP 2 get Shopping Cart count */
			/*                                */
			
			/* Create SQL Command */
			memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
			sprintf(sql_cmd,STMT_addToSC_selSCL,data->sc_id,data->i_id);

			/* Execute SQL Command */
			rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}

			/* Get data */
			SQLBindCol(odbcc->hstmt,1,SQL_INTEGER,&scl_qty,0,NULL);
			rc = SQLFetch(odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			if( rc != SQL_NO_DATA_FOUND ) {
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
					LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
					return W_ERROR;
				}
				
				/* STEP 2.1 UPDATE Shopping Cart Line */
				/* Create SQL Command */
				memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
				sprintf(	sql_cmd,STMT_addToSC_updSCL,
						++scl_qty,
						data->sc_id,
						data->i_id);

				/* Execute SQL Command */
				rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
					LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
					SQLCloseCursor(odbcc->hstmt);
					return W_ERROR;
				}
				SQLCloseCursor(odbcc->hstmt);
			
			} else {
				
				/* STEP 2.2 INSERT Shopping Cart Line */
				/* Create SQL Command */
				memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
				sprintf(sql_cmd,STMT_addToSC_selITM1,data->i_id);

				/* Execute SQL Command */
				rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
					LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
					SQLCloseCursor(odbcc->hstmt);
					return W_ERROR;
				}

				/* Get data */
				SQLBindCol(odbcc->hstmt,1,SQL_C_DOUBLE,&i_cost,0,NULL);
				SQLBindCol(odbcc->hstmt,2,SQL_C_DOUBLE,&i_srp,0,NULL);
				SQLBindCol(odbcc->hstmt,3,SQL_C_CHAR,i_title,sizeof(i_title),NULL);            
				SQLBindCol(odbcc->hstmt,4,SQL_C_CHAR,i_backing,sizeof(i_backing),NULL);	    
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
				sprintf(	sql_cmd,STMT_addToSC_insSCL,
						data->sc_id,
						data->i_id,
						i_cost,
						i_srp,
						i_title,
						i_backing);

				/* Execute SQL Command */
				rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
				if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
					LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
					SQLCloseCursor(odbcc->hstmt);
					return W_ERROR;
				}
				SQLCloseCursor(odbcc->hstmt);
			}
		}
		
	} else {

		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
		sprintf(sql_cmd,STMT_addToSC_selSCLC,data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}

		/* Get data */
		SQLBindCol(odbcc->hstmt,1,SQL_INTEGER,&scl_qty,0,NULL);
		rc = SQLFetch(odbcc->hstmt);
		SQLCloseCursor(odbcc->hstmt);

		if( scl_qty == 0 ) {

			/* Create SQL Command */
			memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ; 
			sprintf(sql_cmd,STMT_addToSC_selITM2,data->i_id);

			/* Execute SQL Command */
			rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}

			/* Get data */
			SQLBindCol(odbcc->hstmt,1,SQL_C_DOUBLE,&i_cost,0,NULL);
			SQLBindCol(odbcc->hstmt,2,SQL_C_DOUBLE,&i_srp,0,NULL);
			SQLBindCol(odbcc->hstmt,3,SQL_C_CHAR,i_title,sizeof(i_title),NULL);         
			SQLBindCol(odbcc->hstmt,4,SQL_C_CHAR,i_backing,sizeof(i_backing),NULL);
			SQLBindCol(odbcc->hstmt,5,SQL_C_LONG,&i_related,0,NULL);
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
			sprintf(	sql_cmd,STMT_addToSC_insSCL,
					data->sc_id,
					( long long int )i_related,
					i_cost,
					i_srp,
					i_title,
					i_backing);

			/* Execute SQL Command */
			rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
			if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
				LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
				SQLCloseCursor(odbcc->hstmt);
				return W_ERROR;
			}
			SQLCloseCursor(odbcc->hstmt);   
		}
	}

	return OK;
}
