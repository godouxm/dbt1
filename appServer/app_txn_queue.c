/*
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <app_txn_queue.h>
#include <common.h>

extern pthread_mutex_t *queue_entry_mutex;

void init_queue(struct Queue *txn_queue, int TxnQSize)
{
	int i;
	txn_queue->front=0;
	txn_queue->rear=0;
	txn_queue->items=0;
	txn_queue->size=TxnQSize;
	txn_queue->capacity=TxnQSize;
	txn_queue->TxnQ=(struct QItem *)malloc(sizeof(struct QItem)*TxnQSize);
	for (i=0; i<TxnQSize; i++)
		txn_queue->TxnQ[i].state=EMPTY;
}

int isempty(struct Queue txn_queue)
{
	if (txn_queue.items==0) return TRUE;
	return FALSE;
}

int isfull(struct Queue txn_queue)
{
	if (txn_queue.capacity==0) return TRUE;
	return FALSE;
}

/* return the number of items in the queue that have not been dequeued */
int queuecount(struct Queue txn_queue)
{
	return txn_queue.items;
}

int enqueue(struct QItem Item, struct Queue *txn_queue)
{
	if (isfull(*txn_queue)) return -1;
	/* find one slot at FRONT that is EMPTY */
	if (!isempty(*txn_queue))
	{
		do
		{
			txn_queue->rear++;
			if (txn_queue->rear==txn_queue->size) txn_queue->rear=0;
		} while (txn_queue->TxnQ[txn_queue->rear].state!=EMPTY);
	}
	else
	{
		while (txn_queue->TxnQ[txn_queue->rear].state!=EMPTY) 
		{
			txn_queue->rear++;
			if (txn_queue->rear==txn_queue->size)
				txn_queue->rear=0;
		}
	}

	txn_queue->TxnQ[txn_queue->rear].state=FULL;
	txn_queue->TxnQ[txn_queue->rear].TxnType=Item.TxnType;
	txn_queue->TxnQ[txn_queue->rear].SlotID=Item.SlotID;
	txn_queue->items++;
	txn_queue->capacity--;

	return txn_queue->rear;
}

int dequeue(struct QItem *Item, struct Queue *txn_queue)
{
	int cur_pos;
	if (isempty(*txn_queue))
		return -1;
	while (txn_queue->TxnQ[txn_queue->front].state!=FULL) 
	{
		txn_queue->front++;
		if (txn_queue->front == txn_queue->size) txn_queue->front=0;
	}
	cur_pos = txn_queue->front;
	Item->TxnType = txn_queue->TxnQ[txn_queue->front].TxnType;
	Item->SlotID = txn_queue->TxnQ[txn_queue->front].SlotID;
	txn_queue->TxnQ[txn_queue->front].state=WAIT;
	txn_queue->items--;
	txn_queue->front++;
	if (txn_queue->front == txn_queue->size)
		txn_queue->front=0;
	if (txn_queue->items == 0)
		txn_queue->rear=txn_queue->front;
 
	return cur_pos;
}

void empty_queue_item_state(struct Queue *txn_queue, int QIndex)
{
	txn_queue->capacity++;
	txn_queue->TxnQ[QIndex].state=EMPTY;
}
