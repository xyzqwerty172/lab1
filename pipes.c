#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 1024

int main() {
    int fd[2];  // File descriptors for pipe
    pid_t pid;
    
    // Create pipe
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(1);
    }
    
    // Create child process
    pid = fork();
    
    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {  // Child process
        printf("Child process running (PID: %d)\n", getpid());
        
        // Close unused read end
        close(fd[READ_END]);
        
        // Duplicate pipe write end to stdout
        // This means anything written to stdout will go to the pipe
        dup2(fd[WRITE_END], STDOUT_FILENO);
        
        // Close original write end as it's no longer needed
        close(fd[WRITE_END]);
        
        // Execute ls -a command
        execlp("ls", "ls", "-a", NULL);
        
        // If execlp returns, it means it failed
        perror("execlp failed");
        exit(1);
    }
    else {  // Parent process
        printf("Parent process running (PID: %d)\n", getpid());
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        
        // Close unused write end
        close(fd[WRITE_END]);
        
        printf("\nOutput from child process (ls -a):\n");
        printf("----------------------------------------\n");
        
        // Read data from pipe until EOF
        while ((bytes_read = read(fd[READ_END], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';  // Null terminate the string
            printf("%s", buffer);
        }
        
        printf("----------------------------------------\n");
        
        // Close read end
        close(fd[READ_END]);
        
        // Wait for child to terminate
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Child process terminated with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process terminated abnormally\n");
        }
    }
    
    return 0;
}