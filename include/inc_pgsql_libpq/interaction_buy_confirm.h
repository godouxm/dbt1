/*
 * interaction_buy_confirm.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd. 
 */

#ifndef _INTERACTION_BUY_CONFIRM_H
#define _INTERACTION_BUY_CONFIRM_H

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_BUYCONF_selSC \
	"SELECT sc_sub_total, sc_tax, sc_ship_cost, sc_total "\
	"FROM   shopping_cart "\
	"WHERE  sc_id = %lld "\
		"AND sc_c_id = %lld;" /* pgxc additional */

#define STMT_BUYCONF_selSC2 \
	"SELECT sc_sub_total, sc_tax, sc_ship_cost, sc_total "\
	"FROM   shopping_cart "\
	"WHERE  sc_id = %lld;"

#define STMT_BUYCONF_selCS \
	"SELECT c_fname, c_lname, c_discount, c_addr_id "\
	"FROM   customer "\
	"WHERE  c_id = %lld;"

#define STMT_BUYCONF_selADRCNT \
	"SELECT addr_id, co_id "\
	"FROM   address, country "\
	"WHERE  co_name      = '%s' "\
		"AND  addr_co_id   = co_id "\
		"AND  addr_zip     = '%s' "\
		"AND  addr_state   = '%s' "\
		"AND  addr_city    = '%s' "\
		"AND  addr_street1 = '%s' "\
		"AND  addr_street2 = '%s' "\
		"AND  addr_c_id    =  %lld;" /* pgxc additional */

#define STMT_BUYCONF_getCountry \
	"SELECT co_id " \
	"FROM country " \
	"WHERE co_name= '%s' ;"

#define STMT_BUYCONF_insADR \
	"INSERT INTO address( addr_id , addr_street1,addr_street2,addr_city,"\
	"addr_state,addr_zip,addr_co_id,addr_c_id) "\
	"VALUES( %lld , '%s', '%s', '%s', '%s', '%s', %d, %lld);" /* pgxc additional */

#define STMT_BUYCONF_selADRID \
	"select nextval('addrid');" 

#define STMT_BUYCONF_selADR \
	"SELECT addr_co_id "\
	"FROM   address "\
	"WHERE  addr_id = %lld "\
		"AND   addr_c_id = %lld;" /* pgxc additional */

#define STMT_BUYCONF_selADR_nocust \
	"SELECT addr_co_id "\
	"FROM   address "\
	"WHERE  addr_id = %lld;"

#define STMT_BUYCONF_insODR \
	"INSERT INTO orders "\
	"VALUES(%lld, %lld, TIMESTAMP'NOW', %lf,"\
		"%lf, %lf, 'SHIP', TIMESTAMP'NOW'+\'%d days\',"\
		"%lld, %lld, 'PENDING');"

#define STMT_BUYCONF_selSCL \
	"SELECT scl_i_id, scl_cost, scl_qty "\
	"FROM   shopping_cart_line "\
	"WHERE  scl_sc_id = %lld;"
/*"AND   scl_c_id  = %lld;"*/ /* pgxc additional */

#define STMT_BUYCONF_insODRL \
	"INSERT INTO order_line "\
	"VALUES(%d, %lld, %lld, %d, %lf, '%s', %lld);" /* pgxc additional */

/* pgxc additional, stock table created */
#define STMT_BUYCONF_selITM \
	"SELECT st_stock FROM stock WHERE st_i_id = %lld;"

/* pgxc additional */
#define STMT_BUYCONF_updITM \
	"UPDATE stock SET st_stock = %d WHERE st_i_id = %lld;"

/* pgxc additional cx_c_id */
#define STMT_BUYCONF_insXACT \
	"INSERT INTO cc_xacts "\
	"VALUES (%lld, '%s', '%s', '%s', '%s', '%s',"\
		"%lf, NOW(), %d, %lld);"

#define STMT_BUYCONF_selInsVal \
	"select nextval('scid');"

int execute_buy_confirm(struct db_context_t *,
	struct buy_confirm_t *);

#endif /* _INTERACTION_BUY_CONFIRM_H */
