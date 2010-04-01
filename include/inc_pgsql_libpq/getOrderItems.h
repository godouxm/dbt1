/*
 * getOrderItems.h 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 * Copyright (C) 2005 Masaki.Kuroda & Shuuichi Suzuki
 *                    UNIADEX, Ltd.
 */

#ifndef _getOrderItems_H
#define _getOrderItems_H

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_getOrderItems \
	"SELECT ol_i_id, i_title, i_publisher, i_cost,"\
		"ol_qty, ol_discount, ol_comments "\
	"FROM item, order_line "\
	"WHERE ol_o_id = %lld "\
		"AND ol_i_id = i_id "\
		"AND ol_c_id = %lld;" /* pgxc additional */

int getOrderItems(struct    db_context_t *,
                  struct    order_display_line_t data[],
                  long long o_id,
		  long long c_id, /* pgxc modif, c_id transfered from order_display_t */
                  short int       *);

#endif /* _geOrderItems_H */
