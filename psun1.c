#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUF_SIZE 5
int buffer[BUF_SIZE];
int in = 0, out = 0;

sem_t empty, full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < 5; i++) {
        int item = id * 100 + i;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        in = (in + 1) % BUF_SIZE;
        printf("Producer %ld produced %d\n", id, item);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < 5; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        out = (out + 1) % BUF_SIZE;
        printf("Consumer %ld consumed %d\n", id, item);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t p1, p2, c1, c2;
    sem_init(&empty, 0, BUF_SIZE);
    sem_init(&full, 0, 0);
    pthread_create(&p1, NULL, producer, (void*)1);
    pthread_create(&p2, NULL, producer, (void*)2);
    pthread_create(&c1, NULL, consumer, (void*)1);
    pthread_create(&c2, NULL, consumer, (void*)2);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}
