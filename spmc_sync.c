/************************
  CS444/544 Operating System

  Week 07: Solving the producer/consumer problem

  Locking scheme description:

    This version is a super silly implementation using a "mother of all
    locks", by simply locking the entire queue on every. single. operation.

    Of course, this is trivially correct: it is impossible to have data races
    because it's impossible to have two references to the queue.

Name: Corey Richardson

Clarkson University  SP2014
 **************************/

#include "spmc_moal.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/***** Defines *****/
#define PRODUCER_SLEEP_S    1
#define CONSUMER_SLEEP_S    1
#define QUEUESIZE           5
#define LOOP               10


/***** Function prototypes *****/
void *producer (void *args);
void *consumer (void *args);

/***** main *****/
int main () {
    spmc_moal_t *fifo = spmc_moal_init(QUEUESIZE);

    //for Consumer's random coming
    unsigned int iseed = (unsigned int)time(NULL);
    srand(iseed);

    /******one producer and multiple consumer********/
    pthread_t pro, con[LOOP];

    if (fifo ==  NULL) {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }
    pthread_create (&pro, NULL, producer, fifo);
    for (int i = 0; i < LOOP; i++) {
        pthread_create (&con[i], NULL, consumer, fifo);
    }

    pthread_join (pro, NULL);

    for (int i = 0; i < LOOP; i++) {
        pthread_join (con[i], NULL);
    }

    spmc_moal_destroy(fifo);

    return 0;
}

/***** producer *****/
void *producer (void *arg) {
    spmc_moal_t *q = (spmc_moal_t*) arg;

    for (int i = 0; i < LOOP; i++) {
        // enqueue already acquires + releases lock for us...  int will fit in
        // a void*, and we never use 0, so we don't bother to actually
        // allocate values.
        spmc_moal_enqueue(q, (void*) i+1);
        printf ("producer: produced %d\n",i+1);

        sleep (PRODUCER_SLEEP_S);
    }

    return (NULL);
}

/***** consumer *****/
void *consumer (void *arg) {
    spmc_moal_t *q = (spmc_moal_t*) arg;

    /* Simulate Consumers' random arrvial */
    sleep (rand()%LOOP);


    sleep (CONSUMER_SLEEP_S);


    int d = 0;
    while (!d) {
        // since we return NULL on an empty queue, make sure we retry. This is
        // a busywait! If threads weren't preempted, this would be a deadlock.
        // The next version of this (spmc_clever) avoids this.
        d = (int) spmc_moal_dequeue(q);
    }
    printf ("------------------------------------>consumer: recieved %d\n", d);

    return (NULL);
}
