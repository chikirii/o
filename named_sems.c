#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define ITEMS_PER_PROD 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0, count = 0;

sem_t *empty, *full, *mutex;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= ITEMS_PER_PROD; i++) {
        int item = id * 100 + i;
        sem_wait(empty);
        sem_wait(mutex);
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("P%d produced %d\n", id, item); fflush(stdout);
        sem_post(mutex);
        sem_post(full);
        usleep(200000);
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(full);
        sem_wait(mutex);
        if (count == 0) { sem_post(mutex); sem_post(full); break; }
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("C%d consumed %d\n", id, item); fflush(stdout);
        sem_post(mutex);
        sem_post(empty);
        usleep(300000);
    }
    return NULL;
}

int main() {
    empty = sem_open("/empty_sem", O_CREAT, 0644, BUFFER_SIZE);
    full  = sem_open("/full_sem",  O_CREAT, 0644, 0);
    mutex = sem_open("/mutex_sem", O_CREAT, 0644, 1);

    int np = 2, nc = 1;
    pthread_t prod[np], cons[nc];
    int ids[np + nc];
    for (int i = 0; i < np; i++) ids[i] = i+1;
    for (int i = 0; i < nc; i++) ids[np+i] = i+1;

    for (int i = 0; i < np; i++) pthread_create(&prod[i], NULL, producer, &ids[i]);
    for (int i = 0; i < nc; i++) pthread_create(&cons[i], NULL, consumer, &ids[np+i]);

    for (int i = 0; i < np; i++) pthread_join(prod[i], NULL);
    for (int i = 0; i < nc; i++) pthread_join(cons[i], NULL);

    sem_close(empty); sem_close(full); sem_close(mutex);
    sem_unlink("/empty_sem"); sem_unlink("/full_sem"); sem_unlink("/mutex_sem");

    return 0;
}
