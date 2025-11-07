#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 5
#define MAX_PRODUCERS 3
#define MAX_CONSUMERS 2
#define MESSAGES_PER_PRODUCER 10

char buffer[BUFFER_SIZE][100];
int buffer_count = 0;
int message_count = 0;

pthread_mutex_t mutex;
pthread_cond_t not_empty;
pthread_cond_t not_full;

void *producer(void *arg) {
    int prod_id = *(int *)arg;
    char message[100];

    for (int i = 0; i < MESSAGES_PER_PRODUCER; i++) {
        snprintf(message, sizeof(message), "Message_%d_from_Producer_%d", i + 1, prod_id);

        pthread_mutex_lock(&mutex);

        while (buffer_count >= BUFFER_SIZE) {
            printf("Producer %d: Buffer full. Waiting...\n", prod_id);
            pthread_cond_wait(&not_full, &mutex);
        }

        strcpy(buffer[buffer_count], message);
        buffer_count++;
        message_count++;

        printf("Producer %d: Produced '%s' (buffer: %d/%d)\n", prod_id, message, buffer_count, BUFFER_SIZE);

        pthread_cond_broadcast(&not_empty);
        pthread_mutex_unlock(&mutex);

        usleep(100000);
    }

    printf("Producer %d: Finished producing all messages\n", prod_id);
    return NULL;
}

void *consumer(void *arg) {
    int cons_id = *(int *)arg;
    char message[100];

    for (int i = 0; i < MESSAGES_PER_PRODUCER; i++) {
        pthread_mutex_lock(&mutex);

        while (buffer_count <= 0) {
            printf("Consumer %d: Buffer empty. Waiting...\n", cons_id);
            pthread_cond_wait(&not_empty, &mutex);
        }

        strcpy(message, buffer[buffer_count - 1]);
        buffer_count--;

        printf("Consumer %d: Consumed '%s' (buffer: %d/%d)\n", cons_id, message, buffer_count, BUFFER_SIZE);

        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&mutex);

        usleep(150000);
    }

    printf("Consumer %d: Finished consuming all messages\n", cons_id);
    return NULL;
}

int main() {
    int num_producers = 3;
    int num_consumers = 2;
    pthread_t prod_threads[MAX_PRODUCERS];
    pthread_t cons_threads[MAX_CONSUMERS];
    int prod_ids[MAX_PRODUCERS];
    int cons_ids[MAX_CONSUMERS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    printf("========== Pthreads Mutex & Condition Variables ==========\n");
    printf("Producers: %d, Consumers: %d\n", num_producers, num_consumers);
    printf("Buffer Size: %d, Messages per Producer: %d\n", BUFFER_SIZE, MESSAGES_PER_PRODUCER);
    printf("=========================================================\n\n");

    for (int i = 0; i < num_producers; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]);
    }

    for (int i = 0; i < num_consumers; i++) {
        cons_ids[i] = i + 1;
        pthread_create(&cons_threads[i], NULL, consumer, &cons_ids[i]);
    }

    for (int i = 0; i < num_producers; i++) {
        pthread_join(prod_threads[i], NULL);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    printf("\n========== Summary ==========\n");
    printf("Total messages produced: %d\n", message_count);
    printf("Final buffer count: %d\n", buffer_count);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return 0;
}

/*
COMPILATION AND EXECUTION COMMANDS:
-----------------------------------
Compile: gcc -o Q5_Q1_mutex_condition Q5_Q1_mutex_condition.c -std=c99 -pthread
Run:     ./Q5_Q1_mutex_condition

Expected Output:
- 3 producers generating 10 messages each
- 2 consumers processing messages
- Buffer size 5 with overflow/underflow protection
- "Buffer full. Waiting..." and "Buffer empty. Waiting..." messages
- Demonstrates mutex and condition variable synchronization
*/
