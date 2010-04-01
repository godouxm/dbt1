/*
 * interaction_order_inquiry.h
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

#ifndef _INTERACTION_ORDER_INQUIRY_H 
#define _INTERACTION_ORDER_INQUIRY_H

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_ORDER_INQUIRY \
	"SELECT c_uname FROM customer WHERE c_id= %lld;"

int execute_order_inquiry(struct db_context_t *,
						  struct order_inquiry_t *);

#endif /* _INTERACTION_ORDER_INQUIRY_H */
