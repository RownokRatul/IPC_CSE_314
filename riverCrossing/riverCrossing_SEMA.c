#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

#define CAPACITY 4
#define SERF_BASE_ID 10000
#define HACKER_BASE_ID 0000
#define NUM_HACKER 5
#define NUM_SERF 12
/*
    forbidden cases: 1 hacker, 3 serf
                     3 hacker, 1 serf
*/

int hacker = 0;
int serf = 0;
int isCaptain = 0;

zem_t hackerQ;
zem_t serfQ;
zem_t mutex;


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

    zem_down(&mutex);
    // printf("mutex acquired %d\n", id);
    hacker++;
    if(hacker == 4) {
        printf("(%d) ready with 4 hackers -> Boarding\n", id);
        for(int i=0;i<3;i++) {
            zem_up(&hackerQ);
        }
        hacker = 0;
        isCaptain = 1;
    }
    else if(hacker == 2 && serf >= 2) {
        printf("(%d) ready with 2 hackers and 2 serfs -> Boarding\n", id);
        for(int i=0;i<2;i++) {
            zem_up(&serfQ);
        }
        zem_up(&hackerQ);
        hacker = 0;
        serf -= 2;
        isCaptain = 1;
    }
    else {
        zem_up(&mutex);
        zem_down(&hackerQ);
        // printf("hacker waking up %d\n", id);
        // printf("Got a signal with Ready Boat %d\n", id);
    }

    if(isCaptain) {
        isCaptain = 0;
        boardAndRow();
        zem_up(&mutex);
    }
}   

void* serfArrives(void* data) {
    int id = *((int*)data);
    printf("serf arrival: %d\n", id);
    zem_down(&mutex);
    // printf("mutex acquired: %d\n", id);
    serf++;
    if(serf == 4) {
        printf("(%d) ready with 4 serfs -> Boarding\n", id);
        for(int i=0;i<3;i++) {
            zem_up(&serfQ);
        }
        serf = 0;
        isCaptain = 1;
    }
    else if(serf == 2 && hacker >= 2) {
        printf("(%d) ready with 2 serfs and 2 hackers -> Boarding\n", id);
        for(int i=0;i<2;i++) {
            zem_up(&hackerQ);
        }
        zem_up(&serfQ);
        serf = 0;
        hacker -= 2;
        isCaptain = 1;
    }
    else {
        zem_up(&mutex);
        zem_down(&serfQ);
        // printf("serf waking up %d\n",id);
    }

    if(isCaptain) {
        isCaptain = 0;
        boardAndRow();
        zem_up(&mutex);
    }
}

int main(int argc, char* argv[]) {
    zem_init(&mutex, 1);
    zem_init(&hackerQ, 0);
    zem_init(&serfQ, 0);

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
        //printf("hacker %d joined\n", hackerID[i]);
    }

    for(int i=0;i<NUM_SERF;i++) {
        pthread_join(serf[i], NULL);
        //printf("serf %d joined\n", serfID[i]);
    }
    printf("\n\nFinished with all crossings!\n");
    return 0;
}