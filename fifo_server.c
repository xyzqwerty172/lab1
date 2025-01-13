// fifo_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/fifo"
#define BUFFER_SIZE 1024

// Function to reverse a string
void reverse_string(char *str) {
    int i, j;
    char temp;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    
    // Create FIFO if it doesn't exist
    if (mkfifo(FIFO_PATH, 0600) == -1) {
        perror("mkfifo failed");
        printf("FIFO might already exist, trying to continue...\n");
    }
    
    printf("Server: Waiting for clients...\n");
    
    // Open FIFO for reading and writing
    fd = open(FIFO_PATH, O_RDWR);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }
    
    while (1) {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Read message from client
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read == -1) {
            perror("read failed");
            break;
        }
        
        // Remove newline if present
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Check if client wants to end
        if (strcmp(buffer, "end") == 0) {
            printf("Server: Received end command. Shutting down...\n");
            break;
        }
        
        printf("Server: Received message: %s\n", buffer);
        
        // Reverse the string
        reverse_string(buffer);
        printf("Server: Sending reversed message: %s\n", buffer);
        
        // Send reversed string back to client
        strcat(buffer, "\n");  // Add newline for client reading
        if (write(fd, buffer, strlen(buffer)) == -1) {
            perror("write failed");
            break;
        }
    }
    
    // Cleanup
    close(fd);
    unlink(FIFO_PATH);
    printf("Server: FIFO closed and removed.\n");
    
    return 0;
}