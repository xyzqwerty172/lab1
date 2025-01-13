#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main() {
    pid_t pid;

    pid = fork(); // Create a child process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process: PID = %d is terminating...\n", getpid());
        exit(0); // Child process terminates immediately
    } else {
        // Parent process
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        printf("Parent is not calling wait(), creating a zombie process...\n");

        // Pause parent to allow the zombie process to be observed
        sleep(30); // Prevent the parent from terminating or reaping the child
        printf("Parent process exiting without reaping child.\n");
    }

    return 0;
}
