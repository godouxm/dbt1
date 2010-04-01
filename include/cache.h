/*
 * cache.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 24 January 2003
 */

#ifndef _CACHE_H_
#define _CACHE_H_

#include "common.h"

struct search_results_record {
	long long i_id;
	char i_title[I_TITLE_LEN+1];
	char a_fname[A_FNAME_LEN+1];
	char a_lname[A_LNAME_LEN+1];
};

struct search_results {
	int record_number;
	struct search_results_record search_results[SEARCH_RESULT_ITEMS_MAX];
	long long i_related[PROMOTIONAL_ITEMS_MAX];
	long long i_thumbnail[PROMOTIONAL_ITEMS_MAX];
};

struct table_range {
	int author_start;
	int author_end;
	int title_start;
	int title_end;
};

#endif /* _CACHE_H_ */
