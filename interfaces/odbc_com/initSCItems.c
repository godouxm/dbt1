/*
 * initSCItems.c
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

#include <strings.h>

#include <interaction_data.h>
#include <initSCItems.h>

int initSCItems(struct shopping_cart_t *init_sc)
{
	int ix;

	for (ix=0;ix<SHOPPING_CART_ITEMS_MAX;ix++) {
		init_sc->scl_data[ix].scl_i_id = (long long)0;
		init_sc->scl_data[ix].scl_qty  = 0;
		init_sc->scl_data[ix].scl_cost = (double)0;
		init_sc->scl_data[ix].scl_srp  = (double)0;
		bzero(init_sc->scl_data[ix].i_title,I_TITLE_LEN);
		bzero(init_sc->scl_data[ix].i_backing,I_BACKING_LEN);
	}

	return OK;
}
