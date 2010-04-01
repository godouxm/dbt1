/*
 * interaction_search_results.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 * Copyright (C) 2005 Koji Fukui
 *                    Sumisho Computer Systems Corporation
 *
 * 26 february 2002
 */

#ifndef _INTERACTION_SEARCH_RESULTS_H_
#define _INTERACTION_SEARCH_RESULTS_H_

#include <interaction.h>
#include <libpq-fe.h>

#define STMT_SEARCH_RESULTS_AUTHOR \
	"SELECT i_id, i_title, a_fname, a_lname "\
	"FROM item, author "\
	"WHERE i_a_id=a_id  "\
		"AND a_lname LIKE '%%%s%%' "\
	"ORDER BY i_title ASC;"

#define STMT_SEARCH_RESULTS_SUBJECT \
	"SELECT i_id, i_title, a_fname, a_lname "\
	"FROM item, author "\
	"WHERE i_subject = '%s' "\
		"AND i_a_id = a_id "\
	"ORDER BY i_title ASC;"

#define STMT_SEARCH_RESULTS_TITLE \
	"SELECT i_id, i_title, a_fname, a_lname "\
	"FROM item, author "\
	"WHERE i_title LIKE '%%%s%%' "\
		"AND i_a_id = a_id "\
	"ORDER BY i_title ASC;"

int execute_search_results(struct db_context_t *,
						   struct search_results_t *);

#endif /* _INTERACTION_SEARCH_RESULTS_H_ */
