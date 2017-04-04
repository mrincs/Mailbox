/*
 * This file contains the code for a extra credit assignment worker thread.
 * The main thread of execution will start a thread by using tring_thread_start.
 * Most of a student's work will go into this file.
 *
 */

#include <stdio.h>
#include <unistd.h>

#include "mailbox.h"
#include "util.h"
#include "tring.h"


void print_message(message *msg, int id) {
    printf("%d : %d\n", id, msg->type);
    return;
}

int checkForIncompleteInfo(int *arr, int len) {
    int i;
    int count = 0;
    for (i = 0; i < len; i++) {
        if (arr[i] == -1)
            return count;
        else
            count++;
    }
    return len;
}

/*
 * inline void tring_signal(void)
 *
 * This is a convenience function for other threads to signal the main thread
 * of execution when important milestones are reached.
 *
 */
void tring_signal(void) {
    pthread_mutex_lock(&main_signal_lock);
    pthread_cond_signal(&main_signal);
    pthread_mutex_unlock(&main_signal_lock);
}

/*
 * void* tring_thread_start(void* arg)
 *
 * This function is used in the creating of worker threads for this assignment.
 * Its parameter 'arg' is a pointer to a thread's designated mailbox.  The
 * thread should exit with a NULL pointer.
 *
 */
void* tring_thread_start(void* arg) {
	int id = -1;
    //int num_of_threads = 4;

    mailbox* mb;
    mailbox* next_mb = NULL;
    
    mb = (mailbox *)arg;
    
    message *msg;
    sleep(1);
    
    /* Message type ID */
    while ((msg = mailbox_receive(mb)) == NULL);
    if (msg->type != ID) {
        //printf("%d Error\n");
        return NULL;
    }
    else {
        printf("%d Received ID message\n", id);
        id = msg->payload.integer;
        FREE_MSG(msg);
    }
    

    
    /* Message type MAILBOX */
    while ((msg = mailbox_receive(mb)) == NULL);
    if (msg->type != MAILBOX) {
        //printf("Error\n");
        return NULL;
    }
    else {
        printf("%d Received MAILBOX message\n", id);
        next_mb = msg->payload.mb;
        FREE_MSG(msg);
    }
    
        

    printf("%x, %d, %x\n", (unsigned int)mb, id, (unsigned int)next_mb);
    
    /* Message type START */
    while ((msg = mailbox_receive(mb)) == NULL);
    if (msg->type != START) {
        printf("Error\n");
        return NULL;
    }
    else {
        printf("%d Received START message\n", id);
        FREE_MSG(msg);
        msg = NEW_MSG;
        msg->type = START;
        msg->payload.mb = NULL;
        msg->payload.integer = -1;
        mailbox_send(next_mb, msg);
    }

    
    /* START received by head again indicating network formation complete */
    if (first_mb == mb) {
        //printf("<first_mb, mb, id>:<%x, %x, %d>\n", first_mb, mb, id);
        while ((msg = mailbox_receive(mb)) == NULL);
        if (msg->type != START) {
            printf("Error\n");
            return NULL;
        }
        else {
            printf("%d Received START message again. Network formation complete\n", id);
            FREE_MSG(msg);
            msg = NEW_MSG;
            msg->type = START;
            msg->payload.mb = NULL;
            msg->payload.integer = -1;
            mailbox_send(next_mb, msg);
        }
    }

    /**** Network formation complete ****/

    // Ring to Linear array
    int id_array[num_of_threads];
    mailbox *mb_array[num_of_threads];
    int i, count;
    int recv_id;
    mailbox *recv_mb;
    
    id_array[0] = id;
    mb_array[0] = mb;
    for (i = 1; i < num_of_threads; i++) {
        id_array[i] = -1;
        mb_array[i] = NULL;
    }
    
        while((count = checkForIncompleteInfo(id_array, num_of_threads)) < num_of_threads) {
            //printf("%d Count: %d\n", id, count);
            // Send Information from own array
            for (i = 0; i < num_of_threads && id_array[i] != -1; i++) {
                msg = NEW_MSG;
                msg->type = INFO;
                msg->payload.mb = mb_array[i];
                msg->payload.integer = id_array[i];
                //printf("%d Sending Info for %d\n", id, id_array[i]);
                mailbox_send(next_mb, msg);
            }
        
            //End message
            msg = NEW_MSG;
            msg->type = INFO;
            msg->payload.mb = next_mb;
            msg->payload.integer = -1;
            mailbox_send(next_mb, msg);
            //printf("%d Sending null info\n", id);
        
        
            int insert_flag;
            // Receive section
            while ((msg = mailbox_receive(mb)) != NULL) {
                insert_flag = 0;
                if(msg->type != INFO) {
                    //printf("%d : Not INFO message\n", id);
                    FREE_MSG(msg);
                    continue;
                }
                recv_id = msg->payload.integer;
                recv_mb = msg->payload.mb;
                // End message received
                if(recv_id == -1 && recv_mb == mb) {
                    //printf("%d End Message\n", id);
                    FREE_MSG(msg);
                    break;
                }
                // Array Traversal for information fill up
                for (i = 0 ; i < num_of_threads; i++) {
                    if (id_array[i] == recv_id) {
                        //printf("%d Already available info: %d\n", id, recv_id);
                        FREE_MSG(msg);
                        break;
                    }
                    else if(id_array[i] == -1){
                        id_array[i] = recv_id;
                        mb_array[i] = recv_mb;
                        //printf("%d : Filling up info: %d\n", id, recv_id);
                        FREE_MSG(msg);
                        break;
                    }
                }
            }
        }
    
    while ((msg = mailbox_receive(mb)) != NULL) {
        FREE_MSG(msg);
    }

    sleep(1);
    // Print the Information
    for (i = 0; i < num_of_threads; i++) {
        printf("%d - %d: %x , ",id, id_array[i], (unsigned int)mb_array[i]);
    }
    printf("\n");
    sleep(1);
    
    // Check for next mailbox for linear chain
    int next_mb_id = id+250;
    int next_mb_index = 0;
    for (i = 1; i < num_of_threads; i++) {
        if (id_array[i] > id) {
            if (id_array[i] < next_mb_id) {
                next_mb_index = i;
                next_mb_id = id_array[i];
            }
        }
    }
    for (i=1;i<num_of_threads;i++){
        if(id_array[i]==next_mb_id){
            next_mb_index=i;
            break;
        }
    }
    if (next_mb_id == id+250) {
        next_mb = NULL;
    }
    else {
        next_mb = mb_array[next_mb_index];
    }
    
    printf("%x, %d, %x %d\n", (unsigned int)mb, id, (unsigned int)next_mb, next_mb_id);
    
    /* Change first_mb according to sorted list */
    if (first_mb == mb) {
        printf("PREV HEAD = %d\n", id);
        int min = id;
        int min_index = 0;
        for (i = 0; i < num_of_threads; i++) {
            if (min > id_array[i]) {
                min = id_array[i];
                min_index = i;
            }
        }
        first_mb = mb_array[min_index];
        printf("Change First_mb to %x\n", (unsigned int)first_mb);
        
        /* Test new formed linear chain*/
        msg = NEW_MSG;
        msg->type = TEST;
        msg->payload.mb = NULL;
        msg->payload.integer = -1;
        mailbox_send(first_mb, msg);
        printf("Testing network\n");
    }
    
    while (((msg = mailbox_receive(mb)) == NULL)||(msg->type == INFO)) {
        FREE_MSG(msg);
    }
    if (msg->type == TEST) {
        printf("%d Received TEST\n", id);
        if (next_mb) {
            mailbox_send(next_mb, msg);
        } else {
            FREE_MSG(msg);
        }
    }

    /**** Network rearrangement complete ****/
    if(first_mb == mb) {
        printf("NEW HEAD = %d\n", id);
        printf("%d Network rearrangement complete. Requesting main to check\n", id);
        tring_signal();
    }

    
    /* Message type PING */
    printf("%d Waiting for PING\n", id);
    while (((msg = mailbox_receive(mb)) == NULL));
    //print_message(msg, id);
    if (msg->type != PING) {
        printf("%d Error\n", id);
        return NULL;
    }
    else {
        printf("%d Received PING message\n", id);
        pong(id);
        if(next_mb) {
            mailbox_send(next_mb, msg);
        } else {
            FREE_MSG(msg);
        }
    }
    
    
    /* Message type PRINT */
    printf("%d Waiting for PRINT\n", id);
    while ((msg = mailbox_receive(mb)) == NULL);
    //print_message(msg, id);
    if (msg->type != PRINT) {
        printf("Error\n");
        return NULL;
    }
    else {
        printf("%d Received PRINT message\n", id);
        if(next_mb) {
            tring_print(id, (int)mb, (int)next_mb);
            mailbox_send(next_mb, msg);
        } else {
            tring_print(id, (int)mb, 0);
            FREE_MSG(msg);
            tring_signal();
        }
    }
    
    /* Message type SHUTDOWN */
    printf("%d Waiting for SHUTDOWN\n", id);
    while ((msg = mailbox_receive(mb)) == NULL);
    //print_message(msg, id);
    if (msg->type != SHUTDOWN) {
        printf("Error\n");
        return NULL;
    }
    else {
        printf("%d Received SHUTDOWN message\n", id);
        if(next_mb) {
            mailbox_send(next_mb, msg);
        } else {
            FREE_MSG(msg);
        }
        mb = NULL;
        next_mb = NULL;
        id = -1;
    }
    
	return NULL;
}
