#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_SIZE 100
#define SHM_KEY 12345

int main() {
    int shmid;
    char *shmaddr;
    pid_t pid;
    char input_string[] = "hello world";
    int i;

    shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {
        shmaddr = (char *)shmat(shmid, NULL, 0);
        
        if (shmaddr == (char *)-1) {
            perror("shmat");
            exit(1);
        }

        strcpy(shmaddr, input_string);
        printf("Process 1 (Parent): Wrote '%s' to shared memory\n", input_string);

        sleep(2);

        printf("Process 1 (Parent): String in shared memory = '%s'\n", shmaddr);

        if (shmdt(shmaddr) == -1) {
            perror("shmdt");
            exit(1);
        }

        wait(NULL);

        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
        }
    }
    else {
        sleep(1);

        shmaddr = (char *)shmat(shmid, NULL, 0);
        
        if (shmaddr == (char *)-1) {
            perror("shmat");
            exit(1);
        }

        printf("Process 2 (Child): Read '%s' from shared memory\n", shmaddr);

        for (i = 0; shmaddr[i] != '\0'; i++) {
            if (shmaddr[i] >= 'a' && shmaddr[i] <= 'z') {
                shmaddr[i] = shmaddr[i] - 32;
            }
        }

        printf("Process 2 (Child): Converted to uppercase = '%s'\n", shmaddr);

        if (shmdt(shmaddr) == -1) {
            perror("shmdt");
            exit(1);
        }

        exit(0);
    }

    return 0;
}

/*
COMPILATION AND EXECUTION COMMANDS:
-----------------------------------
Compile: gcc -o Q2_shared_memory Q2_shared_memory.c -std=c99
Run:     ./Q2_shared_memory

Expected Output:
- Process 1 writes string to shared memory
- Process 2 reads and converts to uppercase
- Both processes show the string transformation
- Demonstrates shared memory IPC
*/
