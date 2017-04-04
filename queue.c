/*
 * This file contains an implementation of a simple FIFO queue.
 */

#include <stdlib.h>

#include "queue.h"

/*
 * void queue_init(queue* q)
 *
 * This function must be called on any queue before it is used.
 *
 */
void queue_init(queue* q) {
	q->first = NULL;
	q->last = NULL;
}

/*
 * int queue_is_empty(queue* q)
 *
 * Returns 1 if the queue is empty, 0 otherwise.
 *
 */
int queue_is_empty(queue* q) {
	if (q->first == NULL) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * void* queue_pop(queue* q)
 *
 * Returns an element from the queue in FIFO order.
 *
 */
void* queue_pop(queue* q) {
	void* ret_payload;
	queue_node* ret_node = q->first;

	if (ret_node == NULL) {
		//This is an empty queue.  Return NULL.
		return NULL;
	} else {
		ret_payload = ret_node->payload;

		if (q->first == q->last) {
			//We now have an empty queue.
			q->first = q->last = NULL;
		} else {
			q->first = ret_node->next;
		}

		//Free the memory that was holding the ret node.
		free(ret_node);

		//Return the pyload.
		return ret_payload;
	}
}

/*
 * void queue_push(queue* q, void* new_payload)
 *
 * Puts an item onto the end of the FIFO queue.
 *
 */
void queue_push(queue* q, void* new_payload) {
	queue_node* new_node;

	new_node = malloc(sizeof(queue_node));
	new_node->payload = new_payload;
	new_node->next = NULL;

	if (q->last == NULL) {
		q->first = q->last = new_node;
	} else {
		q->last->next = new_node;
		q->last = new_node;
	}
}
