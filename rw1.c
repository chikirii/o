#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex, wrt;
int reader_count = 0;
int data = 0;

void *reader(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < 5; ++i) {
        sem_wait(&mutex);
        reader_count++;
        if (reader_count == 1) sem_wait(&wrt);
        sem_post(&mutex);

        printf("Reader %ld reads data = %d\n", id, data);
        sleep(1);

        sem_wait(&mutex);
        reader_count--;
        if (reader_count == 0) sem_post(&wrt);
        sem_post(&mutex);

        sleep(1);
    }
    return NULL;
}

void *writer(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < 3; ++i) {
        sem_wait(&wrt);
        data += (int)id + 1;
        printf("Writer %ld writes data = %d\n", id, data);
        sem_post(&wrt);
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t r1, r2, r3, w1, w2;
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    pthread_create(&r1, NULL, reader, (void*)1);
    pthread_create(&r2, NULL, reader, (void*)2);
    pthread_create(&r3, NULL, reader, (void*)3);
    pthread_create(&w1, NULL, writer, (void*)0);
    pthread_create(&w2, NULL, writer, (void*)1);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(r3, NULL);
    pthread_join(w1, NULL);
    pthread_join(w2, NULL);

    sem_destroy(&mutex);
    sem_destroy(&wrt);
    return 0;
}
