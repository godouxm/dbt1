/*
 * db.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 06 March 2003
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _DB_H_
#define _DB_H_

#include <interaction_data.h>

#ifdef ODBC
#include <interaction.h>
#endif

#ifdef LIBPQ
#include <interaction.h>
#endif

#define STATUS_ROLLBACK 4

#ifdef ODBC
int db_init(char *sname, char *uname, char *auth);
#endif
#ifdef LIBPQ
int db_init(char *sname, char *dbname, char *uname, char *auth, char *dbport);
#endif
int db_connect(struct db_context_t *dbc);
int db_disconnect(struct db_context_t *dbc);
int process_interaction(int interaction, struct db_context_t *dbc,
	union interaction_data_t *data);

#endif /* _DB_H_ */
