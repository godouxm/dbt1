/*
 * createSC.h 
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

#ifndef _createSC_H
#define _createSC_H

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_createSC_selCUST \
	"SELECT c_fname, c_lname, c_discount "\
	"FROM customer "\
	"WHERE c_id=%lld;"

#define STMT_createSC_insSC1 \
	"INSERT INTO shopping_cart(sc_id, sc_c_id, sc_date, "\
		"sc_c_fname, sc_c_lname, sc_c_discount) "\
	"VALUES ( %lld , %lld, NOW() , '%s', '%s', %lf ) ; "

#define STMT_createSC_insSC2 \
	"INSERT INTO shopping_cart(sc_id, sc_date) "\
	"VALUES ( %lld, NOW());"/*, %lld);"*/ /* not necessary to change for pgxc, this sql is launched if shopping_cart_t->c_id == 0 */

#define STMT_createSC_selInsVal \
	"select nextval('scid');"

int createSC(struct db_context_t *,
	struct shopping_cart_t *);

#endif /* _createSC_H */
