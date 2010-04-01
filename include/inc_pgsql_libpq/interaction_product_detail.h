/*
 * interaction_product_detail.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SCS Sumisho Computer System corporation
 *
 * 28 fuary 2002
 * 06 July 2005
 */

#ifndef _INTERACTION_PRODUCT_DETAIL_H_
#define _INTERACTION_PRODUCT_DETAIL_H_

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_PRODUCT_DETAIL \
	"SELECT i_title, a_fname, a_lname, Cast(i_pub_date as char), i_publisher, \
		i_subject, i_desc, i_image, i_cost, i_srp, Cast(i_avail as char), \
		i_isbn, i_page, i_backing, i_dimensions \
	FROM item, author \
	WHERE i_id = %lld \
		AND i_a_id = a_id;"

int execute_product_detail(struct db_context_t *,
						   struct product_detail_t *);

#endif /* _INTERACTION_PRODUCT_DETAIL_H_ */
