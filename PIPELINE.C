#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s prog1 prog2 prog3 file\n", argv[0]);
        exit(1);
    }

    char *prog1 = argv[1];
    char *prog2 = argv[2];
    char *prog3 = argv[3];
    char *file = argv[4];

    int pipefd[2];
    pid_t p1, p2, p3;

    // run prog1
    if ((p1 = fork()) == 0) {
        execlp(prog1, prog1, NULL);
        perror("exec prog1");
        exit(1);
    }
    waitpid(p1, NULL, 0); // wait for prog1

    // pipe between prog2 and prog3
    pipe(pipefd);

    if ((p2 = fork()) == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp(prog2, prog2, NULL);
        perror("exec prog2");
        exit(1);
    }

    if ((p3 = fork()) == 0) {
        int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(pipefd[0], STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp(prog3, prog3, NULL);
        perror("exec prog3");
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);

    return 0;
}
