/*
 * interaction_buy_request.h
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

#ifndef _INTERACTION_BUY_REQUEST_H_
#define _INTERACTION_BUY_REQUEST_H_

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_BUYREQ_updtsc \
	"UPDATE shopping_cart "\
	"SET sc_c_fname    = '%s',"\
		"sc_c_lname    = '%s',"\
		"sc_c_discount = %f,"\
		"sc_c_id       = %lld "\
	"WHERE sc_id = %lld;"

/* pgxc additional SQL command */
#define STMT_BUYREQ_updtscl \
	"UPDATE shopping_cart_line "\
	"SET scl_c_id	= %lld "\
	"WHERE scl_sc_id = %lld;"

/*#define STMT_BUYREQ_updtscl\
 *	"INSERT INTO shopping_cart_line (scl_title,scl_c_id,scl_sc_id) " \
 *	"VALUES ('ca marche',502000,28808);"
 */
int execute_buy_request(struct db_context_t *,
						struct buy_request_t *);

#endif /* _INTERACTION_BUY_REQUEST_H_ */
