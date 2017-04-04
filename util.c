/*
 * This file contains utility functions for the extra credit assignment.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"
#include "util.h"

#define DOT_FILENAME "tring.dot"

static int* ids;
static int current;

static pthread_mutex_t printlock;
static FILE* tring_print_file;

static pthread_mutex_t ponglock;
static int pong_counter;
static queue pong_queue;

/*
 * void ids_cleanup(void)
 *
 * This function frees the memory used by the ids array.
 *
 * THIS FUNCTION IS NOT THREADSAFE.
 *
 */
void ids_cleanup(void) {
	free(ids);
}

/*
 * static int ids_exists(int id, int max)
 *
 * Determins if an id is already in the 'ids' array.
 *
 * THIS FUNCTION IS NOT THREADSAFE.
 *
 */
static int ids_exists(int id, int max) {
	int iter;

	for (iter = 0; iter < max; ++iter) {
		if (ids[iter] == id) {
			return 1;
		}
	}

	return 0;
}

/*
 * void ids_init(int num)
 *
 * Initializes the 'ids' array to contain a series of unique ids between 0 and
 * 5 * num.
 *
 * THIS FUNCTION IS NOT THREADSFE.
 *
 */
void ids_init(int num) {
	int iter, id;

	//Seed our random number generator
	srand(time(NULL));

	current = 0;
	ids = malloc(sizeof(int) * num);

	for (iter = 0; iter < num; ++iter) {
		do {
			id = rand() % (num * 5);
		} while (ids_exists(id, iter));

		ids[iter] = id;
	}
}

/*
 * int ids_next(void)
 *
 * Returns the next id from the 'ids' array.
 *
 * THIS FUNCTION IS NOT THREADSFE.
 *
 */
int ids_next(void) {
	return ids[current++];
}

/*
 * void tring_print(int from, int to)
 *
 * Prints a line to the file 'tring.dot' that will cause an edge to be drawn
 * between the nodes representing the threads with id 'from' and 'to.'
 *
 * This function is threadsafe.
 *
 */
void tring_print(int id, int from, int to) {
	pthread_mutex_lock(&printlock);
	fprintf(tring_print_file, "\t<ID: %d>: %d -> %d;\n", id ,from, to);
	pthread_mutex_unlock(&printlock);

}

/*
 * void tring_print_finalize(void)
 *
 * Prints the closing bracket in the DOT_FILENAME file, and then closes it.
 *
 * This function is threadsafe.
 *
 */
void tring_print_finalize(void) {
	pthread_mutex_lock(&printlock);
	fprintf(tring_print_file, "}\n");
	fclose(tring_print_file);
	pthread_mutex_unlock(&printlock);
}

/*
 * void tring_print_init(void)
 *
 * Initializes the resources need to use the tring_print_* functions.
 *
 * THIS FUNCTION IS NOT THREADSAFE.
 *
 */
void tring_print_init(void) {
	tring_print_file = fopen(DOT_FILENAME, "w");
	pthread_mutex_init(&printlock, NULL);

	fprintf(tring_print_file, "digraph tring_graph {\n");
}

/*
 * void pong(int id)
 *
 * This function is used to record the order of the threads.  The threads should
 * each call this function, in order, passing in their id.
 *
 * This function is threadsafe.
 *
 */
void pong(int id) {
	int* push_id;

	push_id = malloc(sizeof(int));
	*push_id = id;

	pthread_mutex_lock(&ponglock);
	pong_counter++;
    printf("Pong Count : %d, ID = %d\n", pong_counter, id);
	queue_push(&pong_queue, push_id);
	pthread_mutex_unlock(&ponglock);
}

/*
 * int pong_check(void)
 *
 * This function is called to check to see if the threads were successfully
 * arranged in order.
 *
 * This function is threadsafe.
 *
 */
int pong_check(void) {
	int* cur_id;
	int prev_id = -1;

	//Lock the queue.  As this should be called at the end of the program we
	//will never unlock it.
	pthread_mutex_lock(&ponglock);

	while ((cur_id = queue_pop(&pong_queue)) != NULL) {
		if (*cur_id < prev_id) {
			return FALSE;
		} else {
			prev_id = *cur_id;
			free(cur_id);
		}
	}

	return TRUE;
}

/*
 * int pong_count(void)
 *
 * Returns the number of times that pong() has been called.
 *
 * This function is threadsafe.
 *
 */
int pong_count(void) {
	int retval;

	pthread_mutex_lock(&ponglock);
	retval = pong_counter;
	pthread_mutex_unlock(&ponglock);

	return retval;
}

/*
 * void pong_init(void)
 *
 * Initializes the resources used by the pong* calls.
 *
 * THIS FUNCTION IS NOT THREADSAFE.
 *
 */
void pong_init(void) {
	pong_counter = 0;
	pthread_mutex_init(&ponglock, NULL);
	queue_init(&pong_queue);
}
