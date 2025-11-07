#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

long counter = 0;
sem_t mutex;

void *inc(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        sem_wait(&mutex);
        counter++;
        sem_post(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    sem_init(&mutex, 0, 1);
    pthread_create(&t1, NULL, inc, NULL);
    pthread_create(&t2, NULL, inc, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter = %ld\n", counter);
    sem_destroy(&mutex);
    return 0;
}
