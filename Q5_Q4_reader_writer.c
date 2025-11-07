#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int shared_data = 0;
int writer_count = 0;

int read_count = 0;
pthread_mutex_t read_count_mutex;
pthread_mutex_t write_mutex;

void *reader(void *arg) {
    int reader_id = *(int *)arg;

    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&read_count_mutex);
        read_count++;
        if (read_count == 1) {
            pthread_mutex_lock(&write_mutex);
        }
        pthread_mutex_unlock(&read_count_mutex);

        printf("Reader %d: START reading (data = %d)\n", reader_id, shared_data);
        usleep(500000);
        printf("Reader %d: END reading (data = %d)\n", reader_id, shared_data);

        pthread_mutex_lock(&read_count_mutex);
        read_count--;
        if (read_count == 0) {
            pthread_mutex_unlock(&write_mutex);
        }
        pthread_mutex_unlock(&read_count_mutex);

        usleep(200000);
    }

    printf("Reader %d: Finished\n", reader_id);
    return NULL;
}

void *writer(void *arg) {
    int writer_id = *(int *)arg;

    for (int i = 0; i < 2; i++) {
        pthread_mutex_lock(&write_mutex);

        shared_data++;
        printf("Writer %d: START writing (new data = %d)\n", writer_id, shared_data);
        usleep(500000);
        printf("Writer %d: END writing (data = %d)\n", writer_id, shared_data);

        pthread_mutex_unlock(&write_mutex);

        usleep(300000);
    }

    printf("Writer %d: Finished\n", writer_id);
    return NULL;
}

int main() {
    int num_readers = 4;
    int num_writers = 2;
    pthread_t reader_threads[4];
    pthread_t writer_threads[2];
    int reader_ids[4];
    int writer_ids[2];

    pthread_mutex_init(&read_count_mutex, NULL);
    pthread_mutex_init(&write_mutex, NULL);

    printf("========== Reader-Writer Problem ==========\n");
    printf("Readers: %d, Writers: %d\n", num_readers, num_writers);
    printf("Readers can read simultaneously\n");
    printf("Writers get exclusive access\n");
    printf("===========================================\n\n");

    for (int i = 0; i < num_readers; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&reader_threads[i], NULL, reader, &reader_ids[i]);
    }

    for (int i = 0; i < num_writers; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writer_threads[i], NULL, writer, &writer_ids[i]);
    }

    for (int i = 0; i < num_readers; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    for (int i = 0; i < num_writers; i++) {
        pthread_join(writer_threads[i], NULL);
    }

    printf("\n========== Summary ==========\n");
    printf("Final shared data value: %d\n", shared_data);
    printf("Total writes performed: %d\n", shared_data);

    pthread_mutex_destroy(&read_count_mutex);
    pthread_mutex_destroy(&write_mutex);

    return 0;
}

/*
COMPILATION AND EXECUTION COMMANDS:
-----------------------------------
Compile: gcc -o Q5_Q4_reader_writer Q5_Q4_reader_writer.c -std=c99 -pthread
Run:     ./Q5_Q4_reader_writer

Expected Output:
- 4 readers can read simultaneously (parallel START/END messages)
- 2 writers get exclusive access (no other operations during write)
- Clear separation between read and write phases
- Final summary showing total writes performed
- Demonstrates efficient reader-writer synchronization
*/
