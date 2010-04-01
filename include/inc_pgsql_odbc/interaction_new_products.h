/*
 * interaction_new_products.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SCS Sumisho Computer System corporation
 *
 * 21 february 2002
 * 06 July 2005
 */

#ifndef _INTERACTION_NEW_PRODUCTS_H_
#define _INTERACTION_NEW_PRODUCTS_H_

#include <interaction.h>

#define STMT_NEW_PRODUCTS \
	"SELECT i_id, i_title, a_fname, a_lname \
	FROM item, author \
	where i_subject='%s' and i_a_id=a_id \
	ORDER BY i_pub_date DESC, i_title ASC;"

int execute_new_products(struct db_context_t *odbcc,
	struct new_products_t *data);

#endif /* _INTERACTION_NEW_PRODUCTS_H_ */
