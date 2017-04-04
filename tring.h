/*
 * This file contains global resources that may be used in completing this
 * assignment, as well as an convenience function that can be used to signal the
 * main thread of execution at important milestones.
 */

#ifndef TRING_H
#define TRING_H

#include <pthread.h>

#include "mailbox.h"

pthread_cond_t main_signal;
pthread_mutex_t main_signal_lock;

mailbox* first_mb;
extern int num_of_threads;

//inline void tring_signal(void);

#endif
