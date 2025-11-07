#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];
    pid_t pid;
    int arr[] = {10, 20, 30, 40, 50};
    int n = sizeof(arr) / sizeof(arr[0]);
    int sum = 0;
    int i;

    if (pipe(pipe_parent_to_child) == -1) {
        perror("pipe");
        exit(1);
    }

    if (pipe(pipe_child_to_parent) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        write(pipe_parent_to_child[1], &n, sizeof(n));
        write(pipe_parent_to_child[1], arr, n * sizeof(int));
        close(pipe_parent_to_child[1]);

        read(pipe_child_to_parent[0], &sum, sizeof(sum));
        close(pipe_child_to_parent[0]);

        printf("Parent: Array = ");
        for (i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\nParent: Sum received from child = %d\n", sum);

        wait(NULL);
    }
    else {
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        read(pipe_parent_to_child[0], &n, sizeof(n));
        
        int child_arr[n];
        read(pipe_parent_to_child[0], child_arr, n * sizeof(int));
        close(pipe_parent_to_child[0]);

        int child_sum = 0;
        for (i = 0; i < n; i++) {
            child_sum += child_arr[i];
        }

        printf("Child: Received %d elements\n", n);
        printf("Child: Sum computed = %d\n", child_sum);

        write(pipe_child_to_parent[1], &child_sum, sizeof(child_sum));
        close(pipe_child_to_parent[1]);

        exit(0);
    }

    return 0;
}

/*
COMPILATION AND EXECUTION COMMANDS:
-----------------------------------
Compile: gcc -o Q1_pipes Q1_pipes.c -std=c99
Run:     ./Q1_pipes

Expected Output:
- Child receives array and computes sum
- Parent displays original array and received sum
- Demonstrates bidirectional pipe communication
*/
