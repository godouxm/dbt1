/*
 * getCustInfo.h 
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

#ifndef _getCustInfo_H
#define _getCustInfo_H
#include <interaction_data.h>

#define STMT_getCustInfo \
   	"SELECT c_id," \
               "c_passwd," \
               "c_fname," \
               "c_lname," \
               "c_phone," \
               "c_email," \
               "c_data," \
               "c_birthdate," \
               "c_discount," \
               "addr_street1," \
               "addr_street2," \
               "addr_city," \
               "addr_state," \
               "addr_zip," \
               "co_name " \
        "FROM customer, address, country " \
        "WHERE c_uname    = '%s'  and " \
              "addr_id    = c_addr_id and " \
              "addr_co_id = co_id;"

#define STMT_updateCustInfo \
        "UPDATE customer " \
        "SET c_login=now(),"\
            "c_expiration=(now()+'02:00:00') "\
        "WHERE c_id = %lld;"

int getCustInfo(struct db_context_t  *,
                struct buy_request_t *);

#endif /* _getCustInfo_H */
