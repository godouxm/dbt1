/*
 * interaction_search_request.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 * 28 february 2002
 */

#ifndef _INTERACTION_SEARCH_REQUEST_H_
#define _INTERACTION_SEARCH_REQUEST_H_

#include <interaction.h>
#include <libpq-fe.h>

int execute_search_request(struct db_context_t *,
						   struct search_request_t *);

#endif /* _INTERACTION_SEARCH_REQUEST_H_ */
