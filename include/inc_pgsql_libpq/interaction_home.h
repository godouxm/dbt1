/*
 * interaction_home.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 18 february 2002
 */

#ifndef _INTERACTION_HOME_H_
#define _INTERACTION_HOME_H_

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_HOME "SELECT c_fname, c_lname FROM customer WHERE c_id=%lld;"

int execute_home(struct db_context_t *,
				 struct home_t *);

#endif /* _INTERACTION_HOME_H_ */
