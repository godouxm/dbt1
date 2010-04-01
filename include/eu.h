/*
 * eu.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 30 january 2002
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _EU_H_
#define _EU_H_

#include <semaphore.h>

#include <interaction_data.h>

#include <common.h>
#include <interaction.h>

#define MIX_SHOPPING 0
#define MIX_BROWSING 1
#define MIX_ORDERING 2

#define MODE_APPSERVER 0
#define MODE_DIRECT 1

#define MODE_CACHE_OFF 0
#define MODE_CACHE_ON 1

#define MODE_CLEAN_OFF 0
#define MODE_CLEAN_ON 1

#ifdef REALTIME_STATS
#define STAT_INTERVAL 30
#endif

struct eu_context_t
{
	struct db_context_t *dbc;
	int cache_s;

	int s;

	int interaction; /* Current interaction. */
	int previous_interaction; /* The previous interaction. */
	int previous_search_interaction;
	/* indicate if it is the first interaction so that 'prepare_home' can
	 * decide if a new c_id need to be generated
	 */
	int first_interaction; 

	long long c_id;
	long long sc_id;

	struct admin_confirm_t admin_confirm_data;
	struct admin_request_t admin_request_data;
	struct best_sellers_t best_sellers_data;
	struct buy_confirm_t buy_confirm_data;
	struct buy_request_t buy_request_data;
	struct home_t home_data;
	struct new_products_t new_products_data;
	struct product_detail_t product_detail_data;
	struct order_display_t order_display_data;
	struct order_inquiry_t order_inquiry_data;
	struct search_request_t search_request_data;
	struct search_results_t search_results_data;
	struct shopping_cart_t shopping_cart_data;
};

/* Prototypes */

int init_eus(int eus, int interaction_mix, int rampuprate, int duration, 
	double tt_mean, int item_scale);
int mark_logs(char *mark);

extern int customers;
extern int altered;
extern int mode_access;
extern int mode_cache;
extern int mode_clean;
extern char cache_host[32];
extern int cache_port;
extern char sname[32];
extern char dbname[32];
extern char uname[32];
extern char auth[32];
extern int port;
extern char dbport[32];

#endif /* _EU_H_ */
