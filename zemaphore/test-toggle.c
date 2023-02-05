#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

#define NUM_THREADS 3
#define NUM_ITER 10

zem_t mutex;
zem_t cond[NUM_THREADS];
int turn = 0;

// void *justprint(void *data)
// {
//   int thread_id = *((int *)data);
//   // printf("%d\n", thread_id);
//   for(int i=0; i < NUM_ITER; i++)
//     {
//       while (turn != thread_id) {
//         // printf("going to sleep %d\n", thread_id);
//         zem_up(&cond); 
//         zem_down(&cond);
//         // printf("woke up %d\n", thread_id);
//       }
//       printf("%d : This is thread %d\n", i, thread_id);
//       zem_down(&mutex);
//       turn = (turn + 1) % NUM_THREADS;
//       zem_up(&mutex);
//     }
//   return 0;
// }

void *justprint(void *data)
{
  int thread_id = *((int *)data);
  for(int i=0; i < NUM_ITER; i++)
    {
      zem_down(&cond[thread_id]);
      printf("%d : This is thread %d\n", i, thread_id);
      zem_up(&cond[(thread_id + 1) % NUM_THREADS]);
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  zem_init(&mutex, 1);
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      if(!i) {
        zem_init(&cond[i], 1);
      }
      else {
        zem_init(&cond[i], 0);
      }
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
