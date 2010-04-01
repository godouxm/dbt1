/* app_txn_queue.h 
 * transaction queue data structure, it is actually a big array
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */
#ifndef _APP_TXN_QUEUE_H
#define _APP_TXN_QUEUE_H
#include <pthread.h>

/* three states of the QItem: 0 - empty 1 - full, waiting to be dequeued 
 * 2 - being dequeued, but still waiting for the transaction to return
 * from database. This state is used to prevent reusing the slot when
 * database response time is long
 */
#define EMPTY 0
#define FULL 1
#define WAIT 2

struct QItem
{
	int TxnType;
	int SlotID;
	int state;
};

struct Queue
{
	int front;
	int rear;
	/* items is the number of items in the queue that 
         * have not been dequeued*/
	int items;
	int size;
	/* capacity is the number of slots in the queue that is EMPTY*/
	int capacity;
	struct QItem *TxnQ;
};


void init_queue(struct Queue *txn_queue, int TxnQSize);
int isempty(struct Queue txn_queue);
int isfull(struct Queue txn_queue);
int queuecount(struct Queue txn_queue);
int enqueue(struct QItem Item, struct Queue *txn_queue);
int dequeue(struct QItem *Item, struct Queue *txn_queue);
void empty_queue_item_state(struct Queue *txn_queue, int QIndex);

#endif
