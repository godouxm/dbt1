/*
 * app_txn_array.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <app_txn_array.h>

/* array to hold odbc input/output data
 * though the data structure is the same for all the transaction,
 * create an array for each transaction so that there is less contention
 * trying to pin the slot
 */
struct app_txn_array app_admin_confirm_array;
struct app_txn_array app_admin_request_array;
struct app_txn_array app_best_sellers_array;
struct app_txn_array app_buy_confirm_array;
struct app_txn_array app_buy_request_array;
struct app_txn_array app_home_array;
struct app_txn_array app_new_products_array;
struct app_txn_array app_order_display_array;
struct app_txn_array app_order_inquiry_array;
struct app_txn_array app_product_detail_array;
struct app_txn_array app_search_request_array;
struct app_txn_array app_search_results_array;
struct app_txn_array app_shopping_cart_array;

void init_app_txn_array(struct app_txn_array *txn_array, int ArraySize)
{
	int i;
	pthread_mutex_init(&txn_array->txn_array_mutex, NULL);
	txn_array->size = ArraySize;
	txn_array->pin = (int *) malloc(sizeof(int) * ArraySize);
	txn_array->data_array = (union interaction_data_t *)
		malloc(sizeof(union interaction_data_t) * ArraySize);

	txn_array->txn_result = (int *) malloc(sizeof(int) * ArraySize);
#ifdef GET_TIME
	txn_array->db_response_time = (double *) malloc(sizeof(double) * ArraySize);
#endif /* GET_TIME */
	/* all the slots are initially free */
	for (i = 0; i < txn_array->size; i++) 
	{
		txn_array->pin[i] = 0;
		bzero(&txn_array->data_array[i], sizeof(union interaction_data_t));
		txn_array->txn_result[i] = OK;
#ifdef GET_TIME
		txn_array->db_response_time[i] = 0.0;
#endif /* GET_TIME */
	}
}

int PinSlot(struct app_txn_array *txn_array)
{
	int i;
	pthread_mutex_lock(&txn_array->txn_array_mutex);
	for (i = 0; i < txn_array->size; i++)
	{
		if (txn_array->pin[i] == 0) 
		{ 
			txn_array->pin[i] = 1;
			pthread_mutex_unlock(&txn_array->txn_array_mutex);
			return i;
		}
	} 
	/* all are used */
	pthread_mutex_unlock(&txn_array->txn_array_mutex);
	return -1;
}

int FreeSlot(struct app_txn_array *txn_array, int SlotID)
{
	pthread_mutex_lock(&txn_array->txn_array_mutex);
	txn_array->pin[SlotID] = 0;
	bzero(&txn_array->data_array[SlotID], sizeof(union interaction_data_t));
	pthread_mutex_unlock(&txn_array->txn_array_mutex);
	return 1;
}
