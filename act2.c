#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define PIPE_NAME "/tmp/myfifo"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <iterations>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int iterations = atoi(argv[1]);

    // create named pipe
    mkfifo(PIPE_NAME, 0666);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process
        printf("Child begins (pid = %u)\n", pid);
        for (int i = 0; i < iterations; i++)
        {
            int fd = open(PIPE_NAME, O_RDONLY);

            int val;
            read(fd, &val, sizeof(val));
            printf("%u: child: %u\n", i, val);

            close(fd);
        }
        printf("Child ends (pid = %u)\n", pid);
        exit(EXIT_SUCCESS);
    } else { // parent process
        printf("Parent beings (pid = %u)\n", pid);
        for (int i = 0; i < iterations; i++)
        {
            printf("Parent iteration %u (pid = %u)\n", i, pid);
            srand(time(NULL)); // seed the random number generator

            int fd = open(PIPE_NAME, O_WRONLY);

            int rand_val = i;
            write(fd, &rand_val, sizeof(rand_val));
            printf("parent: %u\n", i, rand_val);

            close(fd);
        }
        wait(NULL);

        printf("Parent ends (pid = %u)\n", pid);
        exit(EXIT_SUCCESS);
    }
}