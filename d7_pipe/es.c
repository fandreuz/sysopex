#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

int main()
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    switch (fork())
    {
    case 0:
        //figlio

        if(dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(pipe_fd[0]);

        char *new_arguments[] = {"wc /usr/share/dict/words", "-l", "/usr/share/dict/words"};
        char *current_working_directory = getcwd(NULL, 0);

        execve("/usr/bin/wc", new_arguments, environ);
        perror("execve");

        break;
    case -1:
        perror("fork");
        exit(1);
        break;
    default:
        //padre

        close(pipe_fd[1]);
        if(wait(NULL) == -1) {
            perror("wait");
            exit(1);
        }

        char buffer[10];
        int res = read(pipe_fd[0], buffer, 10);
        if (res <= 0) {
            perror("read");
            exit(1);
        }

        printf("Parole: %d", atoi(buffer));

        break;
    }
}
