/*
 * cache_interface.h 
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#ifndef _CACHE_INTERFACE_H
#define _CACHE_INTERFACE_H_

#include <errno.h>

#include <common.h>
#include <interaction_data.h>
#include <_socket.h>

int receive_search_results(int s,
	struct search_results_t *search_results_request);
int send_search_results(int s, 
	struct search_results_t *search_results_response);

#endif /* _CACHE_INTERFACE_H_ */
