/* app_interface.h 
 * appServer interface for sending/receiving packet to/from eb
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */
#ifndef _APP_INTERFACE_H
#define _APP_INTERFACE_H_

#include "app_txn_queue.h"

int receive_transaction_packet(int s, struct QItem *TxnQItem);
int send_transaction_packet(int s, struct QItem TxnQItem);
#endif
