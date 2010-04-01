/*
 * getSCDetail.h 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 */

#ifndef _getSCDetail_H
#define _getSCDetail_H

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_getSCDetail \
        "SELECT scl_i_id,scl_title,scl_cost,scl_srp,scl_backing,scl_qty "\
        "FROM shopping_cart_line "\
        "WHERE scl_sc_id=%lld;"

int getSCDetail(struct db_context_t  *,
                struct shopping_cart_line_t *,
                long long ,
		long long ,
                short int *);

#endif /* _getSCDetail_H */
