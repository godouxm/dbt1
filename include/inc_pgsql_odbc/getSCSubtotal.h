/*
 * getSCSubtotal.h 
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
#ifndef _getSCSubtotal_H
#define _getSCSubtotal_H

#include <interaction.h>

#define STMT_getSCSubtotal \
       "SELECT SUM(scl_cost*scl_qty) "\
       "FROM shopping_cart_line "\
       "WHERE scl_sc_id=%lld;"

int getSCSubtotal(struct db_context_t  *, long long, double *);

#endif /* _getSCSubtotal_H */
