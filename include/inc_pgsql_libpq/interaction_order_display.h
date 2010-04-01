/*
 * interaction_order_display.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 * 1 march 2002
 */

#ifndef _INTERACTION_ORDER_DISPLAY_H_
#define _INTERACTION_ORDER_DISPLAY_H_

#include <interaction.h>
#include <libpq-fe.h>

/* pgxc modif c_id has to be printed out to be put in order_display */
#define STMT_ORDER_DISPLAY_selODRInfo \
	"SELECT c_id, c_fname, c_lname, c_phone, c_email,"\
		"o_id, o_date, o_sub_total, o_tax,"\
		"o_total, o_ship_type, o_ship_date,"\
		"o_status, o_bill_addr_id, o_ship_addr_id,"\
		"cx_type, cx_auth_id "\
	"FROM customer, orders, cc_xacts "\
	"WHERE c_uname  = '%s' "\
		"AND c_passwd = '%s' "\
		"AND o_c_id   = c_id "\
		"AND cx_o_id  = o_id "\
		"AND cx_c_id  = o_c_id "\
	"ORDER BY o_date DESC;"

/* make a cleanup of order table with recent IDs before displaying them */
#define STMT_ORDER_DISPLAY_delODRInfo \
	"DELETE FROM orders " \
	"WHERE o_date <= (TIMESTAMP'NOW' - interval'5 min') " \
		"AND o_date >= (TIMESTAMP'2010-01-19 23:00') " \
		"AND o_c_id = %lld;"

#define STMT_ORDER_DISPLAY_selADRCNT \
	"SELECT addr_street1, addr_street2, addr_city, "\
		"addr_state, addr_zip, co_name "\
	"FROM address, country "\
	"WHERE addr_id    = %lld "\
		"AND addr_co_id = co_id "\
		"AND addr_c_id  = %lld;" /* pgxc additional */

int execute_order_display(struct db_context_t *,
						  struct order_display_t *);

#endif /* _INTERACTION_ORDER_DISPLAY_H_ */
