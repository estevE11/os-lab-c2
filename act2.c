#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define PIPE_NAME "mypipe"

uint8_t operand1, operand2;
char operation;

void parent(int pipefd[2], int pipefd_conf[2], int iterations) {
    for (int i = 0; i < iterations; i++) {
        // Generate random operands and operation
        operand1 = rand() % 101;
        operand2 = rand() % 101;
        switch (rand() % 4)
        {
        case 0:
            operation = '+';
            break;
        case 1:
            operation = '-';
            break;
        case 2:
            operation = '*';
            break;
        case 3:
            operation = '/';
            break;
        }

        printf("parent (pid = %d): iteration %d.\n", getpid(), i);

        // Write operands and operation to pipe
        write(pipefd[1], &operand1, sizeof(operand1));
        write(pipefd[1], &operation, sizeof(operation));
        write(pipefd[1], &operand2, sizeof(operand2));


        printf("parent (pid = %d): %d %c %d = ?\n", getpid(), operand1, operation, operand2);

        int res = 0;
        read(pipefd_conf[0], &res, sizeof(res));
    }
}

void child(int pipefd[2], int pipefd_conf[2], int iterations) {
    for (int i = 0; i < iterations; i++) {
        uint8_t op1, op2;
        char op;

        // Read operands and operation from pipe
        read(pipefd[0], &op1, sizeof(op1));
        read(pipefd[0], &op, sizeof(op));
        read(pipefd[0], &op2, sizeof(op2));

        printf("child (pid = %d): %d %c %d = ", getpid(), op1, op, op2);

        // Perform mathematical operation
        uint8_t result;
        switch (op)
        {
        case '+':
            result = op1 + op2;
            break;
        case '-':
            result = op1 - op2;
            break;
        case '*':
            result = op1 * op2;
            break;
        case '/':
            result = op1 / op2;
            break;
        }

        printf("%d\n", result);

        int res = 1;
        write(pipefd_conf[1], &res, sizeof(res));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <iterations>\n", argv[0]);
        return 1;
    }

    int iterations = atoi(argv[1]);
    if (iterations <= 0) {
        fprintf(stderr, "Invalid number of iterations.\n");
        return 1;
    }

    srand(time(NULL));

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    int pipefd_conf[2];
    if (pipe(pipefd_conf) == -1) {
        perror("pipe");
        return 1;
    }

    printf("main: created pipe.\n");

    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("child (pid = %d) begins.\n", getpid());
        child(pipefd, pipefd_conf, iterations);
        printf("child (pid = %d) ends.\n", getpid());
    } else {
        printf("main: open pipe for read/write.\n");
        printf("parent (pid = %d) begins.\n", getpid());
        parent(pipefd, pipefd_conf, iterations);
        printf("parent (pid = %d) ends.\n", getpid());
    }

    return 0;
}