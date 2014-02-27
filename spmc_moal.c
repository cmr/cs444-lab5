#include "spmc_moal.h"

#include <stdlib.h>

struct spmc_moal_t {
    pthread_mutex_t moal;
    void **data;
    uintptr_t len;
    uintptr_t max;
};

struct spmc_moal_t *spmc_moal_init(uintptr_t max) {
    struct spmc_moal_t *q = (struct spmc_moal_t*)malloc(sizeof(struct spmc_moal_t));

    if (q == NULL) {
        return NULL;
    }

    pthread_mutex_init(&q->moal, NULL);
    q->data = (void**) malloc(sizeof(void*) * max);
    q->len = 0;
    q->max = max;

    return q;
}

void spmc_moal_destroy(struct spmc_moal_t *q) {
    pthread_mutex_destroy(&q->moal);
    free(q->data);
}

void *spmc_moal_dequeue(struct spmc_moal_t *q) {
    pthread_mutex_lock(&q->moal);
    if (q->len == 0) {
        pthread_mutex_unlock(&q->moal);
        return NULL;
    }

#ifdef DEBUG
    for (int i = 0; i < q->len; i++) {
        printf("0x%x\n", (unsigned int)q->data[i]);
    }
#endif

    void *item = q->data[q->len - 1];
    q->len -= 1;
    pthread_mutex_unlock(&q->moal);
    return item;
}

bool spmc_moal_enqueue(struct spmc_moal_t *q, void *item) {
    pthread_mutex_lock(&q->moal);
    if (q->len == q->max) {
        pthread_mutex_unlock(&q->moal);
        return false;
    }

    q->data[q->len] = item;
    q->len += 1;
    pthread_mutex_unlock(&q->moal);
    return true;
}
