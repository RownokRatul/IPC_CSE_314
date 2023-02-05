#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

#define CAPACITY 4
#define SERF_BASE_ID 10000
#define HACKER_BASE_ID 0000
#define NUM_HACKER 120
#define NUM_SERF 160
/*
    forbidden cases: 1 hacker, 3 serf
                     3 hacker, 1 serf
*/

int hacker = 0;
int serf = 0;
int isCaptain = 0;

// zem_t hackerQ;
// zem_t serfQ;
// zem_t mutex;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t hackerQ = PTHREAD_COND_INITIALIZER;
pthread_cond_t serfQ = PTHREAD_COND_INITIALIZER;


void boardAndRow() {
    printf("----------------------------------------\n");
    printf("Rowing Boat with Boarded People\n");
    // sleep(2);
    printf("Boat Ready Again\n");
    printf("----------------------------------------\n");
}

void* hackerArrives(void* data) {
    int id = *((int*)data);
    printf("hacker arrival: %d\n", id);

    pthread_mutex_lock(&mutex);
    // printf("mutex acquired %d\n", id);
    hacker++;
    if(hacker == 4) {
        printf("(%d) ready with 4 hackers -> Boarding\n", id);
        for(int i=0;i<3;i++) {
            pthread_cond_signal(&hackerQ);
        }
        hacker = 0;
        isCaptain = 1;
    }
    else if(hacker == 2 && serf >= 2) {
        printf("(%d) ready with 2 hackers and 2 serfs -> Boarding\n", id);
        for(int i=0;i<2;i++) {
            pthread_cond_signal(&serfQ);
        }
        pthread_cond_signal(&hackerQ);
        hacker = 0;
        serf -= 2;
        isCaptain = 1;
    }
    else {
        pthread_cond_wait(&hackerQ, &mutex);
        // printf("hacker waking up %d\n", id);
        // printf("Got a signal with Ready Boat %d\n", id);
    }

    if(isCaptain) {
        isCaptain = 0;
        boardAndRow();
    }
    pthread_mutex_unlock(&mutex);
}   

void* serfArrives(void* data) {
    int id = *((int*)data);
    printf("serf arrival: %d\n", id);
    pthread_mutex_lock(&mutex);
    // printf("mutex acquired: %d\n", id);
    serf++;
    if(serf == 4) {
        printf("(%d) ready with 4 serfs -> Boarding\n", id);
        for(int i=0;i<3;i++) {
            pthread_cond_signal(&serfQ);
        }
        serf = 0;
        isCaptain = 1;
    }
    else if(serf == 2 && hacker >= 2) {
        printf("(%d) ready with 2 serfs and 2 hackers -> Boarding\n", id);
        for(int i=0;i<2;i++) {
            pthread_cond_signal(&hackerQ);
        }
        pthread_cond_signal(&serfQ);
        serf = 0;
        hacker -= 2;
        isCaptain = 1;
    }
    else {
        pthread_cond_wait(&serfQ, &mutex);
        // printf("serf waking up %d\n",id);
    }

    if(isCaptain) {
        isCaptain = 0;
        boardAndRow();
    }
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[]) {
    pthread_t hacker[NUM_HACKER];
    pthread_t serf[NUM_SERF];
    int hackerID[NUM_HACKER];
    int serfID[NUM_SERF];
    for(int i=0;i<NUM_HACKER;i++) {
        hackerID[i] = HACKER_BASE_ID + i;
    }
    for(int i=0;i<NUM_SERF;i++) {
        serfID[i] = SERF_BASE_ID + i;
    }

    for(int i=0;i<NUM_HACKER;i++) {
        pthread_create(&hacker[i], NULL, hackerArrives, (void*)&hackerID[i]);
    }

    for(int i=0;i<NUM_SERF;i++) {
        pthread_create(&serf[i], NULL, serfArrives, (void*)&serfID[i]);
    }

    // waiting to join
    for(int i=0;i<NUM_HACKER;i++) {
        pthread_join(hacker[i], NULL);
        // printf("hacker %d joined\n", hackerID[i]);
    }

    for(int i=0;i<NUM_SERF;i++) {
        pthread_join(serf[i], NULL);
        // printf("serf %d joined\n", serfID[i]);
    }

    printf("\n\nFinished with all crossings!\n");
    return 0;
}