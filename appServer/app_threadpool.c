/*
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <app_threadpool.h>
#include <app_txn_queue.h>
#include <app_txn_array.h>
#include <common.h>
#include <_socket.h>
#include <cache_interface.h>
#ifdef ODBC
#include <sql.h>
#include <sqlext.h>
#endif
#include <db.h>

/* mutex to control access to the queue */
extern pthread_mutex_t queue_mutex;

/* semaphore to wait in DoTxn thread, each time a queue item is
 * added to the queue, the semaphore must be released once
 */
extern sem_t TxnQSem;

/* the transaction queue */
extern struct Queue TxnQ;

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

extern struct condition_bundle_t *queue_entry_condition;

extern struct timeval txn_start_time;

extern char search_results_cache_host[32];
extern int search_results_cache_port;

/*
 * Runtime flag to determine whether to use the cache or the database
 * for the search results by author and title.
 */
int mode_cache = 0;

#ifdef ODBC
int init_thread_pool(int PoolThreads, int TxnQSize, char *sname, char *uname,
	char *auth)
#endif
#ifdef LIBPQ
int init_thread_pool(int PoolThreads, int TxnQSize, char *sname, char *dbname, char *uname, char *auth, char *dbport)
#endif
{
	int i;
	pthread_t ThreadID;

	/* create semaphore with initial value 0, and local to current process */
	if (sem_init(&TxnQSem, 0, 0) == -1) return FALSE;
	/* initialize mutex */
	pthread_mutex_init(&queue_mutex, NULL);
	queue_entry_condition = (struct condition_bundle_t *)
		malloc(sizeof(struct condition_bundle_t) * TxnQSize);
	for (i = 0; i < TxnQSize; i++)
	{
		queue_entry_condition[i].done_flag = 0;
		pthread_mutex_init(&queue_entry_condition[i].condition_mutex, NULL);
		pthread_cond_init(&queue_entry_condition[i].done_cv, NULL);
	}

#ifndef _SIMDB
#ifdef ODBC
	/* This should be buried under a generic database initializtion call. */
	if (db_init(sname, uname, auth) == ERROR)
	{
		return ERROR;
	}
#endif
#ifdef LIBPQ
	if (db_init(sname, dbname, uname, auth, dbport) == ERROR)
	{
		return ERROR;
	}
#endif

#endif /* _SIMDB */

	/* Create a pool of threads that connect to the database. */
	printf("Openning connections to the database.\n");
	for (i = 0; i < PoolThreads; i++)
	{
		if (pthread_create (&ThreadID, NULL, DoTxn, NULL) != 0)
		{
			LOG_ERROR_MESSAGE("error creating threadpool thread\n");
			return FALSE;
		}
		/*
		 * We need to slow down the database connection requests because
		 * they will fail if we make too many too fast.
		 */
		sleep(1);
	}
	printf("Database connections have been established.\n");
	return TRUE;
}

void *DoTxn(void *fd)
{
	int rc;
	struct QItem TxnQItem;
	int QIndex;
#ifdef GET_TIME
		struct timeval txn_start_time, txn_end_time;
#endif /* GET_TIME */
	int workersock = 0;

#ifndef _SIMDB
	/* This should be buried under a generic database initialization call. */
	struct db_context_t dbc;
	/* connect to database */
	rc = db_connect(&dbc);
	if (rc == ERROR)
	{
		LOG_ERROR_MESSAGE("db_connect error\n");
		kill(0, SIGUSR1);
		pthread_exit(NULL);
	}
#endif /* _SIMDB */
	/* on Linux we have to init seed in each thread */
	srand(time(NULL) + pthread_self());

	if (mode_cache == 1)
	{
		/* connect to search_results_cache_hsot */
		workersock =
			_connect(search_results_cache_host, search_results_cache_port);
		if (workersock == -1)
		{
			LOG_ERROR_MESSAGE("connect to cache failed\n");
			kill(0, SIGUSR1);
			pthread_exit(NULL);
		}
	}

	/* wait on the TxnQSem. Its value is positive when the queue is not
	 * empty, sem_wait returns. If the queue is empty, block until a job is
	 * enqueued.
	 */

	while (1)
	{
		struct app_txn_array *data = NULL;

		sem_wait(&TxnQSem);

		/* lock the mutex of the TxnQ */
		pthread_mutex_lock(&queue_mutex);
		if ((QIndex = dequeue(&TxnQItem, &TxnQ)) == -1)
		{
			LOG_ERROR_MESSAGE("TxnQ dequeue error");
			pthread_mutex_unlock(&queue_mutex);
			kill(0, SIGUSR1);
			pthread_exit(NULL);
		}
		pthread_mutex_unlock(&queue_mutex);

		/* Point data to the proper transaction array. */
		switch (TxnQItem.TxnType)
		{
			case ADMIN_CONFIRM:
				data = &app_admin_confirm_array;
				break;
			case ADMIN_REQUEST:
				data = &app_admin_request_array;
				break;
			case SEARCH_REQUEST:
				data = &app_search_request_array;
				break;
			case SEARCH_RESULTS:
				data = &app_search_results_array;
				break;
			case BEST_SELLERS:
				data = &app_best_sellers_array;
				break;
			case NEW_PRODUCTS:
				data = &app_new_products_array;
				break;
			case BUY_CONFIRM:
				data = &app_buy_confirm_array;
				break;
			case BUY_REQUEST:
				data = &app_buy_request_array;
				break;
			case HOME:
				data = &app_home_array;
				break;
			case ORDER_DISPLAY:
				data = &app_order_display_array;
				break;
			case ORDER_INQUIRY:
				data = &app_order_inquiry_array;
				break;
			case PRODUCT_DETAIL:
				data = &app_product_detail_array;
				break;
			case SHOPPING_CART:
				data = &app_shopping_cart_array;
				break;
		}
#ifdef GET_TIME
		if (gettimeofday(&txn_start_time, NULL) == -1)
			LOG_ERROR_MESSAGE("gettimeofday failed");
#endif /* GET_TIME */
		if (TxnQItem.TxnType == SEARCH_RESULTS &&
			data->data_array[TxnQItem.SlotID].search_results_data.search_type != SEARCH_SUBJECT &&
			mode_cache == 1)
		{
			/* author and title search results are cached */
			rc = ERROR;
			/* retry if send fails */
			while (rc != OK)
			{
				rc = send_search_results(workersock,
					&app_search_results_array.data_array[TxnQItem.SlotID].search_results_data);
				/* if send fails, reopen a new socket */
				if (rc != OK)
				{
					LOG_ERROR_MESSAGE(
						"send search_results to cache host failed");
					close(workersock);
					workersock = _connect(search_results_cache_host,
						search_results_cache_port);
					if (workersock==-1)
					{
						LOG_ERROR_MESSAGE("connect to cache failed\n");
						kill(0, SIGUSR1);
						pthread_exit(NULL);
					}
				}
			}

			rc = receive_search_results(workersock,
				&app_search_results_array.data_array[TxnQItem.SlotID].search_results_data);
			if (rc != OK)
			{
				LOG_ERROR_MESSAGE(
					"receive search_results from cache host failed");
				close(workersock);
				workersock = _connect(search_results_cache_host,
					search_results_cache_port);
				if (workersock == -1)
				{
					LOG_ERROR_MESSAGE("connect to cache failed\n");
					kill(0, SIGUSR1);
					pthread_exit(NULL);
				}
				rc = ERROR;
			}
		}
		else
		{
			data->txn_result[TxnQItem.SlotID]  =
				process_interaction(TxnQItem.TxnType, &dbc,
				&data->data_array[TxnQItem.SlotID]);
		}
#ifdef GET_TIME
		if (gettimeofday(&txn_end_time, NULL) == -1)
			LOG_ERROR_MESSAGE("gettimeofday failed");
		data->db_response_time[TxnQItem.SlotID] =
			time_diff(txn_start_time, txn_end_time);
#endif /* GET_TIME */

		set_txn_done_flag(&TxnQ, QIndex);
	}
}

void set_txn_done_flag(struct Queue *txn_queue, int QIndex)
{
	pthread_mutex_lock(&queue_entry_condition[QIndex].condition_mutex);
	queue_entry_condition[QIndex].done_flag = 1;
	pthread_cond_signal(&queue_entry_condition[QIndex].done_cv);
	pthread_mutex_unlock(&queue_entry_condition[QIndex].condition_mutex);
	pthread_mutex_lock(&queue_mutex);
	empty_queue_item_state(txn_queue, QIndex);
	pthread_mutex_unlock(&queue_mutex);
}
