#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    pid_t pid;

    // Create a new process
    pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child Process: PID = %d, Parent PID = %d\n", getpid(), getppid());
        printf("Child Process is terminating...\n");
        exit(0); // Child exits
    } else {
        // Parent process
        int status;
        // Wait for the child process to finish
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Parent Process: Child PID %d terminated with status %d\n", pid, WEXITSTATUS(status));
        } else {
            printf("Parent Process: Child PID %d terminated abnormally\n", pid);
        }

        printf("Parent Process: PID = %d is now terminating...\n", getpid());
    }

    return 0;
}
