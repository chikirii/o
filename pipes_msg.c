#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd1[2], fd2[2];
    pipe(fd1);
    pipe(fd2);

    if (fork() == 0) {
        close(fd1[1]);
        close(fd2[0]);

        int arr[5];
        read(fd1[0], arr, sizeof(arr));
        int sum = 0;
        for (int i = 0; i < 5; i++) sum += arr[i];

        write(fd2[1], &sum, sizeof(sum));
        return 0;
    } else {
        close(fd1[0]);
        close(fd2[1]);

        int arr[5] = {1,2,3,4,5}, sum;
        write(fd1[1], arr, sizeof(arr));
        read(fd2[0], &sum, sizeof(sum));
        printf("Sum received from child: %d\n", sum);
        wait(NULL);
    }
    return 0;
}
