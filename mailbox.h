/*
 * This file contains functions declarations for manipulating mailboxes.
 */

#ifndef MAILBOX_H
#define MAILBOX_H

#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

#define NEW_MSG	malloc(sizeof(message));
#define FREE_MSG(msg) free(msg) 

typedef struct mailbox_s {
	queue q;
} mailbox;

enum mtypes {
	ID,
	MAILBOX,
    START,
    INFO,
    TEST,
	PING,
	PRINT,
	SHUTDOWN
};

typedef struct {
	int integer;
	mailbox* mb;
} msg_data;

typedef struct {
	enum mtypes type;
	msg_data payload;
} message;


typedef struct {
    mailbox *mailb;
    int num_of_threads;
}thread_arguments;

pthread_mutex_t mailbox_lock;

void mailbox_init(mailbox* mb);
message* mailbox_receive(mailbox* mb);
void mailbox_send(mailbox* mb, message* msg);

#endif
