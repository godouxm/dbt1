/*
 * app_txn_array.h 
 * array of odbc_data_t to hold input/output for each transaction
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#ifndef _APP_TXN_ARRAY_H_
#define _APP_TXN_ARRAY_H_

#include <pthread.h>
#include <interaction_data.h>

struct app_txn_array
{
	int size;
	int *pin;
	pthread_mutex_t txn_array_mutex;
	union interaction_data_t *data_array;
	int *txn_result;
#ifdef GET_TIME
	double *db_response_time;
#endif
};

/* the txn input output array */
extern struct app_txn_array app_admin_confirm_array;
extern struct app_txn_array app_admin_request_array;
extern struct app_txn_array app_best_sellers_array;
extern struct app_txn_array app_buy_confirm_array;
extern struct app_txn_array app_buy_request_array;
extern struct app_txn_array app_home_array;
extern struct app_txn_array app_new_products_array;
extern struct app_txn_array app_order_display_array;
extern struct app_txn_array app_order_inquiry_array;
extern struct app_txn_array app_product_detail_array;
extern struct app_txn_array app_search_request_array;
extern struct app_txn_array app_search_results_array;
extern struct app_txn_array app_shopping_cart_array;

void init_app_txn_array(struct app_txn_array *txn_array, int ArraySize);
int PinSlot(struct app_txn_array *txn_array);
int FreeSlot(struct app_txn_array *txn_array, int SlotID);

#endif /* _APP_TXN_ARRAY_H_ */
