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

int refreshSC(struct db_context_t *dbc,
	struct shopping_cart_t *data)
{
	int ix = 0 ;
	char sql_cmd[512];

	PGresult *res;

	for (ix = 0;ix < data->sc_size;ix++)
	{
		/* Create SQL Command */
		memset(sql_cmd, 0x00, sizeof(sql_cmd));
		if (data->sc_refresh[ix].scl_qty == 0) {
			sprintf(sql_cmd,
				STMT_refreshSC_del,
				data->sc_id,
				data->sc_refresh[ix].scl_i_id,
				data->c_id);
		}
		else
		{
			sprintf(sql_cmd,
				STMT_refreshSC_upd,
				data->sc_refresh[ix].scl_qty,
				data->sc_id,
				data->sc_refresh[ix].scl_i_id,
				data->c_id);
		}

		/* Execute SQL Command */
		res = PQexec(dbc->conn, sql_cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			LOG_ERROR_MESSAGE("%s\nPQbackendPID[%d]\nPQstatus[%d]\nPQtransactionStatus[%d]\nPQsocket[%d]\nPQprotocolVersion[%d]\n", PQerrorMessage(dbc->conn), PQbackendPID(dbc->conn), (int)PQstatus(dbc->conn), (int)PQtransactionStatus(dbc->conn), PQsocket(dbc->conn), PQprotocolVersion(dbc->conn));
			PQclear(res);
			return ERROR;
		}
		PQclear(res);
	}

	return OK;
}
