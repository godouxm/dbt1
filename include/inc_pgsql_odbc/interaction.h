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

#ifndef _ODBC_INTERACTION_H_
#define _ODBC_INTERACTION_H_

/* comment the next line out if unixODBC is used */
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string.h>

#include <common.h>
#include <interaction_data.h>

#define LOG_ODBC_ERROR(type, handle) odbc_error(__FILE__, __LINE__, type, handle)

struct db_context_t
{
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
};

#endif /* _ODBC_INTERACTION_H_ */

/* Prototypes */
int _db_connect(struct db_context_t *odbcc);
int _db_disconnect(struct db_context_t *odbcc);
int odbc_error(char *filename, int line, SQLSMALLINT handle_type,
	SQLHANDLE handle);
int _db_init(char *sname, char *uname, char *auth);
int commit_transaction(struct db_context_t *dbc);
int rollback_transaction(struct db_context_t *dbc);

#endif /* _INTERACTION_H_ */
