/*
 * interaction_buy_request.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 */

#include <interaction_buy_request.h>
#include <getCustInfo.h>
#include <InsertCust.h>
#include <updateSC.h>
#include <getSCDetail.h>

int execute_buy_request(	struct db_context_t *odbcc,
						struct buy_request_t *data)
{
	SQLRETURN rc;
	char sql_cmd[512];
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		
	/* check customer status */
	if (data->returning_flag == 1) {
		rc = getCustInfo(odbcc,data);
		if (rc == W_ERROR) {
			return W_ERROR;
		}
	} else {

		/* first time */
		rc = InsertCust(odbcc,data);
		if (rc == W_ERROR) {
			return W_ERROR;
		}

		/* update shopping cart */
		/* Create SQL Command */
		sprintf(sql_cmd,STMT_BUYREQ_updtsc ,
					data->c_fname,
					data->c_lname,
					data->c_discount,
					data->c_id,
					data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);
	}

	rc = updateSC(odbcc,data);
	if (rc == W_ERROR) {
		return W_ERROR;
	}

	rc = getSCDetail(odbcc,&data->scl_data[0],data->sc_id,&data->sc_size);
	if (rc == W_ERROR) {
		return W_ERROR;
	}

	return OK;
}
