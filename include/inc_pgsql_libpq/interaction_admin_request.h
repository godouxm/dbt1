/*
 * interaction_admin_request.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SCS Sumisho Computer System corporation
 *
 * 28 february 2002
 * 06 July 2005
 */

#ifndef _INTERACTION_ADMIN_REQUEST_H_
#define _INTERACTION_ADMIN_REQUEST_H_

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_ADMIN_REQUEST \
	"SELECT i_srp, i_cost, i_title, i_image, i_thumbnail, a_fname, a_lname \
	FROM item, author \
	WHERE i_id = %lld AND i_a_id = a_id;"

int execute_admin_request(struct db_context_t *,
	struct admin_request_t *);

#endif /* _INTERACTION_ADMIN_REQUEST_H_ */
