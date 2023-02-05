#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "zemaphore.h"

void zem_init(zem_t *s, int value) {
    s->value = value;
    // s->lock = PTHREAD_MUTEX_INITIALIZER;
    // s->cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_init(&s->cond, NULL);
    pthread_mutex_init(&s->mutex, NULL);
}

void zem_down(zem_t *s) {
    pthread_mutex_lock(&s->mutex);
    while(s->value <= 0) {
        pthread_cond_wait(&s->cond, &s->mutex);
    }
    s->value--;
    pthread_mutex_unlock(&s->mutex);
}

void zem_up(zem_t *s) {
    pthread_mutex_lock(&s->mutex);
    s->value++;
    // printf("yes waking\n");
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->mutex);
}
