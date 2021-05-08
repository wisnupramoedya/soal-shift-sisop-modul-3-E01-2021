#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void closeAll(int fd[][2]) {
    close(fd[0][0]);
    close(fd[0][1]);
    close(fd[1][0]);
    close(fd[1][1]);
}

int main() {
    int status;
    int fd[2][2];
    char *ps_arg[] = {"ps", "aux", NULL};
    char *sort_arg[] = {"sort", "-nrk", "3,3", NULL};
    char *head_arg[] = {"head", "-5", NULL};

    pipe(fd[0]);
    pipe(fd[1]);

    if (fork() == 0) {
        dup2(fd[0][1], STDOUT_FILENO);
        closeAll(fd);
        execv("/bin/ps", ps_arg);
    } else if (fork() == 0) {
        dup2(fd[0][0], STDIN_FILENO);
        dup2(fd[1][1], STDOUT_FILENO);
        closeAll(fd);
        execv("/bin/sort", sort_arg);
    } else if (fork() == 0) {
        dup2(fd[1][0], STDIN_FILENO);
        closeAll(fd);
        execv("/bin/head", head_arg);
    }
    closeAll(fd);

    for (int i = 0; i < 3; i++) wait(&status);
    return 0;
}