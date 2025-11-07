#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int shmid = shmget(IPC_PRIVATE, 1024, 0666 | IPC_CREAT);
    char *str = (char*) shmat(shmid, NULL, 0);

    if (fork() == 0) {
        sleep(1);
        for (int i = 0; str[i]; i++) str[i] = toupper((unsigned char)str[i]);
        printf("Child: Converted to uppercase: %s\n", str);
        shmdt(str);
        return 0;
    } else {
        strcpy(str, "hello shared memory");
        printf("Parent: Wrote string: %s\n", str);
        wait(NULL);
        shmdt(str);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}
