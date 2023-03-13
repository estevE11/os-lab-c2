#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2];
    pid_t cpid;
    char buffer[10];

    // create pipe
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // fork child process
    cpid = fork();

    if (cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0)
    {                     // child process
        close(pipefd[1]); // close unused write end of the pipe

        // read 3 integers from the pipe
        int num;
        for (int i = 0; i < 3; i++)
        {
            read(pipefd[0], &num, sizeof(num));
            printf("Child received number %d\n", num);
        }

        close(pipefd[0]); // close read end of the pipe
        _exit(EXIT_SUCCESS);
    }
    else
    {                     // parent process
        close(pipefd[0]); // close unused read end of the pipe

        // generate 3 random integers and send to the child process
        for (int i = 0; i < 3; i++)
        {
            int num = rand() % 100;
            printf("Parent sent number %d\n", num);
            write(pipefd[1], &num, sizeof(num));
        }

        close(pipefd[1]); // close write end of the pipe

        // wait for child process to terminate
        wait(NULL);

        exit(EXIT_SUCCESS);
    }
}