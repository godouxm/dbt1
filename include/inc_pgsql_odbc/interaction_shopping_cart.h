/*
 * interaction_shopping_cart.h
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

#ifndef _INTERACTION_SHOPPING_CART_H
#define _INTERACTION_SHOPPING_CART_H

#include <interaction.h>

#define STMT_SC_upd \
       "UPDATE shopping_cart "\
          "SET sc_sub_total=%lf, sc_date=NOW() "\
       "WHERE sc_id=%lld;"

int execute_shopping_cart(struct db_context_t *odbcc,
                          struct shopping_cart_t *data);

#endif /* _INTERACTION_SHOPPING_CART_H */
