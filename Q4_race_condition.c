#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

long counter = 0;
pthread_mutex_t counter_mutex;
int use_mutex = 0;

void *increment_counter(void *arg) {
    int thread_id = *(int *)arg;
    
    for (int i = 0; i < 1000000; i++) {
        if (use_mutex) {
            pthread_mutex_lock(&counter_mutex);
        }

        counter++;

        if (use_mutex) {
            pthread_mutex_unlock(&counter_mutex);
        }
    }

    printf("Thread %d finished incrementing\n", thread_id);
    return NULL;
}

void run_without_mutex() {
    printf("\n========== Running WITHOUT Mutex (Race Condition) ==========\n");
    counter = 0;
    use_mutex = 0;

    pthread_t thread1, thread2;
    int tid1 = 1, tid2 = 2;

    pthread_create(&thread1, NULL, increment_counter, &tid1);
    pthread_create(&thread2, NULL, increment_counter, &tid2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Expected counter value: 2000000\n");
    printf("Actual counter value: %ld\n", counter);
    printf("Difference (lost updates): %ld\n", 2000000 - counter);
}

void run_with_mutex() {
    printf("\n========== Running WITH Mutex (Synchronized) ==========\n");
    counter = 0;
    use_mutex = 1;

    pthread_t thread1, thread2;
    int tid1 = 1, tid2 = 2;

    pthread_mutex_init(&counter_mutex, NULL);

    pthread_create(&thread1, NULL, increment_counter, &tid1);
    pthread_create(&thread2, NULL, increment_counter, &tid2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Expected counter value: 2000000\n");
    printf("Actual counter value: %ld\n", counter);
    printf("Difference (lost updates): %ld\n", 2000000 - counter);

    pthread_mutex_destroy(&counter_mutex);
}

int main() {
    printf("Race Condition Demonstration\n");
    printf("Two threads incrementing a shared counter 1000000 times each\n");

    run_without_mutex();
    run_with_mutex();

    printf("\n========== Analysis ==========\n");
    printf("Without mutex: Race condition causes lost updates\n");
    printf("With mutex: Synchronization ensures all increments are counted\n");

    return 0;
}

/*
COMPILATION AND EXECUTION COMMANDS:
-----------------------------------
Compile: gcc -o Q4_race_condition Q4_race_condition.c -std=c99 -pthread
Run:     ./Q4_race_condition

Expected Output:
- First run without mutex shows lost updates (race condition)
- Second run with mutex shows correct count (2,000,000)
- Clear demonstration of race condition vs synchronization
- Analysis section explaining the difference
*/
