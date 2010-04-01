/*
 * interaction_search_request.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 * 28 february 2002
 */

#include <interaction_search_request.h>
#include <getPromoImages.h>

int execute_search_request(	struct db_context_t *odbcc,
						struct search_request_t *data)
{
	SQLRETURN rc;

	/* Generate random number for Promotional Processing. */
	data->pp_data.i_id = (UDWORD) get_random_int(item_count) + 1;

	rc = getPromoImages(odbcc,&data->pp_data);

	return OK;
}
