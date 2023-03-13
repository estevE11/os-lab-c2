#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>

#define SHARED_MEM_SIZE 4 // 4 bytes

int main()
{
    // Create shared memory
    int shm_fd = shm_open("/activity2_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    if (ftruncate(shm_fd, SHARED_MEM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    int *shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Create semaphores
    sem_t *sem1 = sem_open("/activity2_sem1", O_CREAT | O_EXCL, 0666, 0);
    if (sem1 == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }
    
    sem_t *sem2 = sem_open("/activity2_sem2", O_CREAT | O_EXCL, 0666, 0);
    if (sem2 == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Seed random number generator
    srand(time(NULL));

    // Initialize shared memory with random number
    *shared_mem = rand() % 100;

    // Create child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // child process
        while (*shared_mem > 0) {
            sem_wait(sem1);  // wait for parent's turn
            if (*shared_mem > 0) {
                (*shared_mem)--; // decrement shared memory value
                printf("Child process decremented shared memory to %d\n", *shared_mem);
            }
            sem_post(sem2); // signal parent's turn
        }
        exit(0);
    }
    else { // parent process
        while (*shared_mem > 0) {
            (*shared_mem)--; // decrement shared memory value
            printf("Parent process decremented shared memory to %d\n", *shared_mem);
            sem_post(sem1); // signal child's turn
            sem_wait(sem2); // wait for child's turn
        }
    }

    // Free resources
    sem_close(sem1);
    sem_close(sem2);
    munmap(shared_mem, SHARED_MEM_SIZE);
    close(shm_fd);
    shm_unlink("/activity2_shm");
    sem_unlink("/activity2_sem1");
    sem_unlink("/activity2_sem2");

    return 0;
}