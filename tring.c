/*
 * This file contains the main function for the Extra Credit assignment.
 * Students will need to make some modifications to this file.  Places where
 * changes WILL need to be made are labeled with FIXME.  Other changes may be
 * made to meet the assignment requirements.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mailbox.h"
#include "tring.h"
#include "util.h"
#include "tring_thread.h"

/*
 * void tring_protocol_start(mailbox* mb)
 *
 * In this function, students will provide code that will cause the threads to
 * start passing whatever messages they need to to meet the assignment goals.
 *
 */

int num_of_threads;

void tring_protocol_start(mailbox* mb) {
    printf("Main: Sending first START\n");
    message *msg;
    msg = NEW_MSG;
    msg->type = START;
    msg->payload.mb = NULL;
    msg->payload.integer = -1;
    mailbox_send(mb, msg);
}

/*
 * static inline void tring_wait(void)
 *
 * This is a convenience function for waiting on the threads.  This is
 * accomplished by waiting on a condition variable.
 *
 */
static inline void tring_wait(void) {
	pthread_mutex_lock(&main_signal_lock);
	pthread_cond_wait(&main_signal, &main_signal_lock);
	pthread_mutex_unlock(&main_signal_lock);
}



/*
 * mailbox* spawn_thread(mailbox* previous_mb)
 *
 * This function is responsible for starting new threads.  It returns a pointer
 * to the mailbox of the newly created thread.  If a mailbox is given as an
 * argument, this mailbox is given to the new thread as the value for its
 * 'next_mb' variable.
 *
 * Students will need to make a change to this function.
 *
 */
mailbox* spawn_thread(mailbox* previous_mb, int num_threads) {
	pthread_t thread;

	mailbox* current_mb;
	message* msg;

	//Initialize the new thread's mailbox.
	current_mb = malloc(sizeof(mailbox));
	mailbox_init(current_mb);

	//Create the new therad.
	/*
	 * FIXME
	 *
	 * Replace this comment with a call to pthread_create that will create a
	 * new thread using the function tring_thread_start().
	 */

    
    if(pthread_create(&thread, NULL, tring_thread_start, current_mb)) {
        fprintf(stderr, "Error creating thread. \n");
        free(current_mb);
        return NULL;
    }

	//Make a new ID message for the new thread.
	msg = NEW_MSG;
	msg->type = ID;
	msg->payload.integer = ids_next();
    msg->payload.mb = NULL;
    //printf("Main: Sending ID message\n");

	//Send the message.
	mailbox_send(current_mb, msg);

	if (previous_mb != NULL) {
		//Make a new MAIBLOX message.
		msg = NEW_MSG;
		msg->type = MAILBOX;
		msg->payload.mb = previous_mb;
        msg->payload.integer = -1;
        //printf("Main: Sending MAILBOX message\n");
		//Send the mssage.
		mailbox_send(current_mb, msg);
	}

	return current_mb;
}


/*
 * int main(int argc, char** argv)
 *
 * The main thread of execution for Programming Assignment 2.  It creates the
 * threads, starts the threads to work, asseses the correctness of the solution,
 * and causes them to print out a graph of their connections, then causes them
 * to shutdown.
 *
 * Students may need to make changes to this function.
 *
 */
int main(int argc, char** argv) {
	int iter;
    int num_threads;

	mailbox* previous_mb;
	message* msg;

	if (argc == 2) {
		num_threads = atoi(argv[1]);
	} else {
		printf("Usage: ./tring <N>\n\t N: Number of threads to spawn.\n");
		exit(0);
	}

	/********** Initialization Code **********/

	//Initialize the main_signal mutex and CV.
	pthread_mutex_init(&main_signal_lock, NULL);
	pthread_cond_init(&main_signal, NULL);

	//Initialize the utility functions.
	ids_init(num_threads);
	tring_print_init();
	pong_init();
    num_of_threads = num_threads;

	/*********** Thread Spawning Code **********/

	//Create the first thread.
	first_mb = previous_mb = spawn_thread(NULL, num_threads);

	//Create the rest of the threads.
	for (iter = 1; iter < num_threads; ++iter) {
		previous_mb = spawn_thread(previous_mb, num_threads);
	}

	//Give the first thread the mailbox of the last one created.
	msg = NEW_MSG;
	msg->type = MAILBOX;
	msg->payload.mb = previous_mb;
    msg->payload.integer = -1;
	mailbox_send(first_mb, msg);
    printf("Main: Sending last MAILBOX\n");

	/********** Protocol Start Code **********/

	//Run the start code.
	tring_protocol_start(first_mb);

	//Wait for the threads to finish up their business.
	tring_wait();

	/********** Finalization Code **********/
	//Have the threads report in.
    printf("Main: Sending PING message to first_mb\n");
	msg = NEW_MSG;
	msg->type = PING;
	mailbox_send(first_mb, msg);

	//Have the threads print themselves out.
    printf("Main: Sending PRINT message to first_mb\n");
	msg = NEW_MSG;
	msg->type = PRINT;
	msg->payload.integer = -1;
	mailbox_send(first_mb, msg);

	//Wait for the threads to finish ponging and printing.
	tring_wait();

	//Check to see if we have a correct solution.
	if (pong_count() == num_threads) {
		printf("All threads reachable.\n");

		if (pong_check()) {
			printf("All threads in order.\n\nSolution is correct!\n");
		} else {
			printf("One or more threads are out of order.\nIncorrect solution.\n\n");
		}
	} else {
		printf("Only %d of %d threads reachable.\n\nIncorrect solution.\n", pong_count(), num_threads);
	}

	//Finalize the graph and clean up the ids.
	tring_print_finalize();
	ids_cleanup();

	/*********** Thread Shutdown Code **********/
	msg = NEW_MSG;
	msg->type = SHUTDOWN;
	msg->payload.mb = NULL;
	mailbox_send(first_mb, msg);

	return 0;
}
