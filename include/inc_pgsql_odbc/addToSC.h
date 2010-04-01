/*
 * addToSC.h 
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

#ifndef _addToSC_H
#define _addToSC_H

#include <interaction.h>

#define STMT_addToSC_selsumSCL \
       "SELECT sum(scl_qty) "\
       "FROM  shopping_cart_line "\
       "WHERE scl_sc_id=%lld "\
       "HAVING sum(scl_qty) IS NOT NULL;"

#define STMT_addToSC_selSCL \
       "SELECT scl_qty "\
       "FROM shopping_cart_line "\
       "WHERE scl_sc_id=%lld "\
         "AND scl_i_id =%lld;"

#define STMT_addToSC_updSCL \
       "UPDATE shopping_cart_line "\
       "SET scl_qty=%d "\
       "WHERE scl_sc_id=%lld "\
         "AND scl_i_id =%lld"

#define STMT_addToSC_selITM1 \
       "SELECT i_cost, i_srp, i_title, i_backing "\
       "FROM  item "\
       "WHERE i_id=%lld;"

#define STMT_addToSC_insSCL \
       "INSERT INTO shopping_cart_line "\
       "VALUES(%lld, %lld, 1, %lf, %lf, '%s', '%s');"

       
#define STMT_addToSC_selSCLC \
       "SELECT count(scl_sc_id) "\
       "FROM   shopping_cart_line "\
       "WHERE  scl_sc_id=%lld;"

#define STMT_addToSC_selITM2 \
       "SELECT t1.i_cost, t1.i_srp, t1.i_title, t1.i_backing, t2.i_related1 "\
       "FROM  item t1, item t2 "\
       "WHERE t1.i_id=t2.i_related1 "\
         "AND t2.i_id=%lld;"

int addToSC(struct db_context_t *odbcc,
            struct shopping_cart_t *data);

#endif /* _addToSC_H */
