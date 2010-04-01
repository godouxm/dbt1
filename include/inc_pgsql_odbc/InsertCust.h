/*
 * InsertCust.h 
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

#ifndef _InsertCust_H
#define _InsertCust_H
#include <interaction_data.h>

#define STMT_InsertCust_getCountry \
        "SELECT co_id "\
        "FROM country "\
        "WHERE co_name='%s';"

#define STMT_InsertCust_getAddr \
        "SELECT addr_id "\
        "FROM address "\
        "WHERE addr_co_id   =  %d "\
          "AND addr_zip     = '%s' "\
          "AND addr_state   = '%s' "\
          "AND addr_city    = '%s' "\
          "AND addr_street1 = '%s' "\
          "AND addr_street2 = '%s';"

#define STMT_InsertCust_insAddr \
        "INSERT INTO address (addr_id,"\
			     "addr_street1,"\
                             "addr_street2,"\
                             "addr_city,"\
                             "addr_state,"\
                             "addr_zip,"\
                             "addr_co_id) "\
        "VALUES ( %d , "\
		"'%s',"\
                "'%s',"\
                "'%s',"\
                "'%s',"\
                "'%s',"\
                "%d);"

#define STMT_InsertCust_insAddr_selInsVal \
        "select nextval('addrid');"

#define STMT_InsertCust_LastCID \
        "select c_id from customer order by c_id desc limit 1;"\

#define STMT_InsertCust_insCust \
        "INSERT INTO customer"\
                  "(c_id,      c_uname,     c_passwd, c_fname,"\
                   "c_lname,   c_addr_id,   c_phone,  c_email,"\
                   "c_since,   c_last_visit,c_login,  c_expiration,"\
                   "c_discount,c_balance,   c_ytd_pmt,c_birthdate,"\
                   "c_data) "\
        "VALUES( %lld ,'%s',LOWER('%s'),'%s',"\
               "'%s',%d,'%s',"\
               "'%s',NOW(),NOW(),"\
               "TIMESTAMP'NOW',(NOW()+'02:00:00'),%f,"\
               "0.00, 0.00, '%s','%s');"

#define STMT_InsertCust_selInsVal \
        "select nextval('custid');" 

int InsertCust(struct db_context_t  *,
               struct buy_request_t *);

#endif /* _InsertCust_H */
