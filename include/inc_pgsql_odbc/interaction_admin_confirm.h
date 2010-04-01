/*
 * interaction_admin_confirm.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Hongwei Liu@SCS Sumisho Computer System corporation
 *
 * 1 march 2002
 * 06 July 2005
 */

#ifndef _INTERACTION_ADMIN_CONFIRM_H_
#define _INTERACTION_ADMIN_CONFIRM_H_

#include <interaction.h>

//after update, does the DBMS need COMMIT??
#define STMT_ADMIN_CONFIRM_UPD \
	"UPDATE item set i_image=%lld, i_thumbnail=%lld, i_cost=%lf, i_pub_date=Date(Now()) \
	WHERE i_id = %lld;"

#define STMT_ADMIN_CONFIRM_RET \
        "SELECT i_image, i_thumbnail, i_cost, i_title, a_fname, a_lname,\
        i_subject, i_desc, i_srp, i_backing, i_page,\
        i_publisher, i_pub_date, i_dimensions, i_isbn \
        From item, author \
        WHERE i_id = %lld and i_a_id = a_id;"

int execute_admin_confirm(struct db_context_t *odbcc,
    struct admin_confirm_t *data);

#endif /* _INTERACTION_ADMIN_CONFIRM_H_ */
