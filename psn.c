#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_NAME "/pc_shm"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define SEM_MUTEX "/sem_mutex"
#define BUF_SIZE 5

typedef struct {
    int buf[BUF_SIZE];
    int in, out;
} shared_t;

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    int producer = strcmp(argv[1], "producer") == 0;
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shared_t));
    shared_t *s = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sem_t *empty = sem_open(SEM_EMPTY, O_CREAT, 0666, BUF_SIZE);
    sem_t *full = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    sem_t *mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    if (producer) {
        for (int i = 1; i <= 10; i++) {
            sem_wait(empty);
            sem_wait(mutex);
            s->buf[s->in] = i;
            printf("Produced %d at %d\n", i, s->in);
            s->in = (s->in + 1) % BUF_SIZE;
            sem_post(mutex);
            sem_post(full);
            sleep(1);
        }
    } else {
        for (int i = 1; i <= 10; i++) {
            sem_wait(full);
            sem_wait(mutex);
            int val = s->buf[s->out];
            printf("Consumed %d from %d\n", val, s->out);
            s->out = (s->out + 1) % BUF_SIZE;
            sem_post(mutex);
            sem_post(empty);
            sleep(2);
        }
        sem_unlink(SEM_EMPTY);
        sem_unlink(SEM_FULL);
        sem_unlink(SEM_MUTEX);
        shm_unlink(SHM_NAME);
    }
    return 0;
}
