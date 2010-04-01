/*
 * updateSC.h 
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

#ifndef _updateSC_H
#define _updateSC_H

#include <interaction.h>

#define STMT_updateSC_sel \
       "SELECT scl_i_id, scl_qty, i_cost "\
       "FROM shopping_cart_line, item "\
       "WHERE i_id=scl_i_id "\
         "AND scl_sc_id=%lld;"

#define STMT_updateSC_updscl \
       "UPDATE shopping_cart_line "\
       "SET scl_cost=%lf "\
       "WHERE scl_i_id=%lld AND scl_sc_id=%lld;"

#define STMT_updateSC_updsc \
       "UPDATE shopping_cart "\
       "SET sc_date=NOW(), sc_sub_total=%f,"\
           "sc_tax=%f, sc_ship_cost=%f,"\
           "sc_total=%f "\
       "WHERE sc_id=%lld;"

int updateSC(struct db_context_t  *,
             struct buy_request_t *);

#endif /* _updateSC_H */
