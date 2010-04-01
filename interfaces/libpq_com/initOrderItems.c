/*
 * initOrderItems.c 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 */

#include <string.h>

#include <interaction_order_display.h>
#include <initOrderItems.h>

int initOrderItems(struct order_display_line_t data[])
{
	int ix;

	for (ix = 0;ix < SHOPPING_CART_ITEMS_MAX;ix++)
	{
		data[ix].ol_i_id = 0;
		memset(data[ix].i_title,'\0',sizeof(data[ix].i_title));
		memset(data[ix].i_publisher,'\0',sizeof(data[ix].i_publisher));
		data[ix].i_cost = 0;
		data[ix].ol_qty = 0;
		data[ix].ol_discount = 0;
		memset(data[ix].ol_comment,'\0',sizeof(data[ix].ol_comment));
	}

	return OK;
}
