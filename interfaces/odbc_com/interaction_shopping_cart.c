/*
 * interaction_shopping_cart.c
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

#include <interaction_shopping_cart.h>
#include <createSC.h>
#include <addToSC.h>
#include <refreshSC.h>
#include <getSCSubtotal.h>
#include <getSCDetail.h>
#include <getPromoImages.h>

int execute_shopping_cart(	struct db_context_t *odbcc,
						struct shopping_cart_t *data)
{

	SQLRETURN rc;
	double    sc_subtotal = 0 ;
	char sql_cmd[512];
	memset( sql_cmd , 0x00 , sizeof( sql_cmd ) ) ;

	/* generate random item if add_flag == FALSE and empty (i_id, qty) pair */
	if (data->sc_size == 0 && data->add_flag == FALSE) {
		data->i_id = (long long)get_random_int(item_count) + 1;
	}

	/* Generate random number for Promotional Processing. */
	data->pp_data.i_id = (long long)get_random_int(item_count) + 1;

	if (data->add_flag == 1 ||
		(data->add_flag == 0 && data->sc_size ==0 && data->sc_id == 0) ||
		(data->add_flag == 0 && data->sc_size !=0)) {

		/* STEP 1 Create Shopping Cart */
		if (data->sc_id == 0) {
			rc = createSC(odbcc,data);
			if (rc == W_ERROR) {
				return W_ERROR;
			}
		}

		if (data->add_flag == 1 || (data->add_flag == 0 && data->sc_size == 0)) {
			rc = addToSC(odbcc,data);
			if (rc == W_ERROR) {
				return W_ERROR;
			}
		} else {
			rc = refreshSC(odbcc,data);
			if (rc == W_ERROR) {
				return W_ERROR;
			}
		}

		rc = getSCSubtotal(odbcc,data->sc_id,&sc_subtotal);
		if (rc == W_ERROR) {
			return W_ERROR;
		}

		/* Create SQL Command */
		sprintf(sql_cmd,STMT_SC_upd,sc_subtotal,data->sc_id);

		/* Execute SQL Command */
		rc = SQLExecDirect(odbcc->hstmt,sql_cmd,SQL_NTS);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			LOG_ODBC_ERROR(SQL_HANDLE_STMT, odbcc->hstmt);
			SQLCloseCursor(odbcc->hstmt);
			return W_ERROR;
		}
		SQLCloseCursor(odbcc->hstmt);
	}

	rc = getSCDetail(odbcc,&data->scl_data[0],data->sc_id,&data->sc_size);
	if (rc == W_ERROR) {
		return W_ERROR;
	}

	rc = getPromoImages(odbcc,&data->pp_data);
	if (rc == W_ERROR) {
		return W_ERROR;
	}

	return OK;
}
