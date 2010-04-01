/*
 * refreshSC.c 
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
#include <refreshSC.h>

int refreshSC(	struct db_context_t    *odbcc,
			struct shopping_cart_t *data)
{
	SQLRETURN rc;
	int ix = 0 ;
	char sql_cmd[512];
	
	for (ix = 0;ix < data->sc_size;ix++) {

		/* Create SQL Command */
		memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;
		if (data->sc_refresh[ix].scl_qty == 0) {
			sprintf(	sql_cmd,
					STMT_refreshSC_del,
					data->sc_id,
					data->sc_refresh[ix].scl_i_id);
		} else {
			sprintf(	sql_cmd,
					STMT_refreshSC_upd,
					data->sc_refresh[ix].scl_qty,
					data->sc_id,
					data->sc_refresh[ix].scl_i_id);
		}

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);
	}

	return OK;
}
