#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        close(fd[1]);
        int value;
        while (1) {
            ssize_t r = read(fd[0], &value, sizeof(value));
            if (r == 0) break;
            if (r == -1) { perror("read"); break; }
            printf("Reader: got %d\n", value);
        }
        close(fd[0]);
        return 0;
    } else {
        close(fd[0]);
        for (int i = 1; i <= 10; ++i) {
            if (write(fd[1], &i, sizeof(i)) != sizeof(i)) { perror("write"); break; }
            printf("Writer: sent %d\n", i);
            sleep(1);
        }
        close(fd[1]);
        wait(NULL);
        return 0;
    }
}
