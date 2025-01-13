#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_NAME "/my_shm"
#define SHM_SIZE 1024

int main() {
    // Create and configure shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    // Set the size of shared memory object
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    // Map shared memory to process address space
    void *shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    // Fork a child process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }
    else if (pid == 0) {
        // Child process
        printf("Child process running (PID: %d)\n", getpid());
        
        // Write message to shared memory
        const char *message = "Greeting to parent!";
        memcpy(shm_ptr, message, strlen(message) + 1);
        
        printf("Child wrote message to shared memory\n");
        
        // Cleanup and exit
        munmap(shm_ptr, SHM_SIZE);
        close(shm_fd);
        exit(0);
    }
    else {
        // Parent process
        printf("Parent process running (PID: %d)\n", getpid());
        
        // Wait for child to terminate
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Child process terminated successfully\n");
            printf("Message from shared memory: %s\n", (char *)shm_ptr);
        } else {
            printf("Child process terminated with error\n");
        }
        
        // Cleanup
        munmap(shm_ptr, SHM_SIZE);
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }

    return 0;
}