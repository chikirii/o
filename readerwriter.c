#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t wrt;
pthread_mutex_t mutex;
int readcount = 0;
int data_val = 0;

void* reader(void* arg) {
    int id = *(int*)arg;
    for (int k=0;k<5;k++) {
        pthread_mutex_lock(&mutex);
        readcount++;
        if (readcount == 1) sem_wait(&wrt);
        pthread_mutex_unlock(&mutex);

        printf("Reader %d is reading data = %d\n", id, data_val);
        sleep(1);

        pthread_mutex_lock(&mutex);
        readcount--;
        if (readcount == 0) sem_post(&wrt);
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    for (int k=0;k<5;k++) {
        sem_wait(&wrt);
        data_val++;
        printf("Writer %d modified data to %d\n", id, data_val);
        sleep(1);
        sem_post(&wrt);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t rtid[3], wtid[2];
    int rid[3], wid[2];

    sem_init(&wrt, 0, 1);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 3; i++) {
        rid[i] = i+1;
        pthread_create(&rtid[i], NULL, reader, &rid[i]);
    }
    for (int i = 0; i < 2; i++) {
        wid[i] = i+1;
        pthread_create(&wtid[i], NULL, writer, &wid[i]);
    }

    for (int i = 0; i < 3; i++) pthread_join(rtid[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(wtid[i], NULL);

    sem_destroy(&wrt);
    pthread_mutex_destroy(&mutex);
    return 0;
}
