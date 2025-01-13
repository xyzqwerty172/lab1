#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100

int main() {
    char input[MAX_INPUT];  // Buffer to store user input
    char *args[MAX_ARGS];   // Array of arguments for execvp
    pid_t pid;
    int background = 0;     // Flag to check if the command runs in the background

    while (1) {
        printf("os> "); // Command prompt
        fflush(stdout);

        // Read user input
        ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input) - 1);
        if (bytesRead <= 0) {
            perror("Error reading input");
            break;
        }

        input[bytesRead - 1] = '\0'; // Null-terminate the input string

        // Parse the input into tokens
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the args array

        // Check if the command should run in the background
        if (i > 0 && strcmp(args[i - 1], "&") == 0) {
            background = 1;
            args[i - 1] = NULL; // Remove '&' from the arguments
        } else {
            background = 0;
        }

        // Fork a child process
        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("Execution failed");
                exit(1);
            }
        } else {
            // Parent process
            if (!background) {
                // Wait for the child process to finish if not running in the background
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    printf("Child process %d exited with status %d\n", pid, WEXITSTATUS(status));
                } else {
                    printf("Child process %d did not exit normally\n", pid);
                }
            } else {
                printf("Child process %d running in background\n", pid);
            }
        }
    }

    return 0;
}
