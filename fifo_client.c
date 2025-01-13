// fifo_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/fifo"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    
    // Open FIFO for reading and writing
    printf("Client: Waiting for server...\n");
    fd = open(FIFO_PATH, O_RDWR);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }
    
    printf("Client: Connected to server.\n");
    
    while (1) {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Get input from user
        printf("Enter a string (or 'end' to quit): ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            perror("fgets failed");
            break;
        }
        
        // Remove newline if present
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Send message to server
        strcat(buffer, "\n");  // Add newline for server reading
        if (write(fd, buffer, strlen(buffer)) == -1) {
            perror("write failed");
            break;
        }
        
        // Remove newline for comparison
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Check if we're ending
        if (strcmp(buffer, "end") == 0) {
            printf("Client: Shutting down...\n");
            break;
        }
        
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Read reversed string from server
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read == -1) {
            perror("read failed");
            break;
        }
        
        printf("Received reversed string: %s", buffer);
    }
    
    // Cleanup
    close(fd);
    printf("Client: FIFO closed.\n");
    
    return 0;
}