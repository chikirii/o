#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int buffer_count = 0;
int producer_count = 0;
int consumer_count = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void *producer(void *arg) {
    int prod_id = *(int *)arg;
    int item;

    for (int i = 0; i < 3; i++) {
        item = prod_id * 100 + i;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[buffer_count] = item;
        buffer_count++;
        producer_count++;

        printf("Producer %d: Produced item %d (buffer count: %d)\n", prod_id, item, buffer_count);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);
    }

    return NULL;
}

void *consumer(void *arg) {
    int cons_id = *(int *)arg;
    int item;

    for (int i = 0; i < 3; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        item = buffer[buffer_count - 1];
        buffer_count--;
        consumer_count++;

        printf("Consumer %d: Consumed item %d (buffer count: %d)\n", cons_id, item, buffer_count);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t prod1, prod2, cons1, cons2;
    int prod1_id = 1, prod2_id = 2;
    int cons1_id = 1, cons2_id = 2;

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    printf("Producer-Consumer Problem with Semaphores\n");
    printf("Buffer Size: %d\n\n", BUFFER_SIZE);

    pthread_create(&prod1, NULL, producer, &prod1_id);
    pthread_create(&prod2, NULL, producer, &prod2_id);
    pthread_create(&cons1, NULL, consumer, &cons1_id);
    pthread_create(&cons2, NULL, consumer, &cons2_id);

    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    printf("\nTotal items produced: %d\n", producer_count);
    printf("Total items consumed: %d\n", consumer_count);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}

/*
COMPILATION AND EXECUTION COMMANDS:
-----------------------------------
Compile: gcc -o Q3_producer_consumer Q3_producer_consumer.c -std=c99 -pthread
Run:     ./Q3_producer_consumer

Expected Output:
- 2 producers and 2 consumers working with buffer
- Proper synchronization with semaphores
- Buffer count tracking and thread completion messages
- Demonstrates classic producer-consumer problem
*/
