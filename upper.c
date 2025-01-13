#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

// Error handling wrapper functions
pid_t Fork(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork error");
        exit(1);
    }
    return pid;
}

void Pipe(int fd[2]) {
    if (pipe(fd) < 0) {
        perror("Pipe error");
        exit(1);
    }
}

ssize_t Read(int fd, void *buf, size_t count) {
    ssize_t n;
    if ((n = read(fd, buf, count)) < 0) {
        perror("Read error");
        exit(1);
    }
    return n;
}

void Write(int fd, const void *buf, size_t count) {
    if (write(fd, buf, count) < 0) {
        perror("Write error");
        exit(1);
    }
}

int main() {
    int p_to_c[2];  // Parent to child pipe
    int c_to_p[2];  // Child to parent pipe
    pid_t pid;
    char buffer[1024];
    
    // Create both pipes
    Pipe(p_to_c);
    Pipe(c_to_p);
    
    pid = Fork();
    
    if (pid == 0) {  // Child process
        // Close unused pipe ends
        close(p_to_c[1]);  // Close write end of parent-to-child pipe
        close(c_to_p[0]);  // Close read end of child-to-parent pipe
        
        printf("Child process running (PID: %d)\n", getpid());
        
        char ch;
        ssize_t n;
        
        // Read characters from parent, convert to uppercase, and send back
        while ((n = Read(p_to_c[0], &ch, 1)) > 0) {
            printf("Child received: %c\n", ch);
            
            // Convert to uppercase and send back to parent
            ch = toupper(ch);
            Write(c_to_p[1], &ch, 1);
        }
        
        // Close remaining pipe ends
        close(p_to_c[0]);
        close(c_to_p[1]);
        
        printf("Child process terminating\n");
        exit(0);
    }
    else {  // Parent process
        // Close unused pipe ends
        close(p_to_c[0]);  // Close read end of parent-to-child pipe
        close(c_to_p[1]);  // Close write end of child-to-parent pipe
        
        printf("Parent process running (PID: %d)\n", getpid());
        
        // Message to send to child
        const char *message = "Hello, child process!\n";
        printf("\nParent sending message: %s", message);
        
        // Send message to child character by character
        for (size_t i = 0; message[i] != '\0'; i++) {
            Write(p_to_c[1], &message[i], 1);
            usleep(100000);  // Small delay to make the output more readable
        }
        
        // Close write end to signal EOF to child
        close(p_to_c[1]);
        
        printf("\nConverted message from child:\n");
        
        // Read converted characters from child
        char ch;
        ssize_t n;
        while ((n = Read(c_to_p[0], &ch, 1)) > 0) {
            printf("%c", ch);
        }
        
        // Close remaining pipe end
        close(c_to_p[0]);
        
        // Wait for child to terminate
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("\nChild process terminated with status %d\n", WEXITSTATUS(status));
        } else {
            printf("\nChild process terminated abnormally\n");
        }
    }
    
    return 0;
}