/*
 * tm_interface.h
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#ifndef _TM_INTERFACE_H
#define _TM_INTERFACE_H
#include <eu.h>

int receive_interaction_packet(int s, struct eu_context_t *euc);
int send_interaction_packet(int s, struct eu_context_t *euc);

#endif
