/*
 * cache_interface.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 18 march 2002
 */

#include "cache_interface.h"

int receive_search_results(int s, struct search_results_t
	*search_results_request)
{
	int length;
	int rec;

	/* Receive search_results request data. */
	length = sizeof(struct search_results_t);

	if ((rec = _receive(s, search_results_request, length)) == -1)
	{
		LOG_ERROR_MESSAGE("cannot receive search results request");
		return ERROR;
	}
	if (rec == 0)
	{
		return SOCKET_CLOSE;
	}

	return OK;
}

int send_search_results(int s, 
	struct search_results_t *search_results_response)
{
	int length;

	length = sizeof(struct search_results_t);
	if (_send(s, search_results_response, length) == -1)
	{
		LOG_ERROR_MESSAGE("cannot send search_results, errno %d", errno);
		return ERROR;
	}

	return OK;
}

