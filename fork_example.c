#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void forkChildren(int nChildren);

int main() {
    int n = 3; // Number of child processes to create
    forkChildren(n);
    return 0;
}

void forkChildren(int nChildren) {
    for (int i = 1; i <= nChildren; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            // Fork failed
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            printf("I'm a child: %d PID: %d, my parent PID: %d\n", i, getpid(), getppid());
            exit(0); // Child exits after printing
        }
        // Parent process continues the loop
    }

    // Parent waits for all children to finish
    for (int i = 1; i <= nChildren; i++) {
        wait(NULL);
    }
}
