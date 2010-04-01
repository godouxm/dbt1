/*
 * app_threadpool.h 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _APP_THREADPOOL_H
#define _APP_THREADPOOL_H_

#ifdef ODBC
#include <interaction.h>
#endif
#ifdef PGSQL
#include <pgsql_interaction.h>
#endif

#include <app_txn_queue.h>

struct condition_bundle_t {
	int done_flag;
	pthread_mutex_t condition_mutex;
	pthread_cond_t done_cv;
};

#ifdef ODBC
int init_thread_pool(int PoolThreads, int TxnQSize, char *sname, char *uname,
	char *auth);
#endif
#ifdef LIBPQ
int init_thread_pool(int PoolThreads, int TxnQSize, char *sname, char *dbname, char *uname, char *auth, char *dbport);
#endif
void *DoTxn(void *fd);
void set_txn_done_flag(struct Queue *txn_queue, int QIndex);

#ifdef _SIMDB
void fill_promo_data(struct promotional_processing_t *pp_data);
#endif /* _SIMDB */

extern int mode_cache;

#endif /* _APP_THREADPOOL_H_ */
