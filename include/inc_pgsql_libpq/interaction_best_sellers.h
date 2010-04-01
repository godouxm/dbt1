/*
 * interaction_best_sellers.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SUMISHO Computer Systems Corporation.
 *
 * 26 february 2002
 * 06 July 2005
 */

#ifndef _INTERACTION_BEST_SELLERS_H_
#define _INTERACTION_BEST_SELLERS_H_

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_BEST_SELLERS \
	"SELECT i_id, i_title, a_fname, a_lname \
	FROM item, author \
	WHERE i_subject = '%s' \
		AND i_a_id = a_id \
	ORDER BY i_pub_date DESC, i_title ASC;"

int execute_best_sellers(struct db_context_t *,
						 struct best_sellers_t *);

#endif /* _INTERACTION_BEST_SELLERS_H_ */
