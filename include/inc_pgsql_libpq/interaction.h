/*
 * interaction.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#ifndef _INC_PGSQL_INTERACTION_H_
#define _INC_PGSQL_INTERACTION_H_

#include <string.h>
#include <common.h>
#include <interaction_data.h>
#include <libpq-fe.h>

#define LOG_ODBC_ERROR(type, handle) odbc_error(__FILE__, __LINE__, type, handle)

struct db_context_t {
	PGconn *conn;
};

int _db_connect(struct db_context_t *);
int _db_disconnect(struct db_context_t *);
int _db_init(char *, char *, char *, char *, char *);
int commit_transaction(struct db_context_t *);
int rollback_transaction(struct db_context_t *);
int begin_transaction(struct db_context_t *);

#endif /* _INC_PGSQL_INTERACTION_H_ */
