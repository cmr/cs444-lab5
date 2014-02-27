#ifndef SPMC_MOAL_H
#define SPMC_MOAL_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

// type-safe, abi-safe interface. unfortunately introduces indirection in the
// form of void pointers. easy to avoid with templates.

// opaque type
typedef struct spmc_moal_t spmc_moal_t;

// Create a new spmc_moal_t, with a max length of @max. Note that this is a
// bounded queue, which will be allocated up-front.
spmc_moal_t *spmc_moal_init(uintptr_t max);
// Destroy (free) a queue. This is unsafe, as anything contained in it will
// leak, and it doesn't acquire the lock. Beware!!!
void spmc_moal_destroy(spmc_moal_t *queue);
// Dequeue an item. Returns NULL when empty queue. This is blocking.
void *spmc_moal_dequeue(spmc_moal_t *queue);
// Attempt to enqueue an item. Returns false if the queue is full, true
// otherwise. This is blocking.
bool spmc_moal_enqueue(spmc_moal_t *queue, void *item);

#endif
