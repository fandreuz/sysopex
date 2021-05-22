#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        // child

        if (close(pipe_fd[0]) == -1)
        {
            perror("close");
            exit(1);
        }

        // produco un risultato
        sleep(2);

        char *buffer = "ciaooo";
        int res = write(pipe_fd[1], buffer, strlen(buffer));

        if (close(pipe_fd[1]) == -1)
        {
            perror("pipe");
            exit(1);
        }

        exit(0);
        break;
    case -1:
        perror("fork");
        exit(1);
    default:
        // parent
        if (close(pipe_fd[1]) == -1)
        {
            perror("close");
            exit(1);
        }

        break;
    }

    int pipe2_fd[2];

    if (pipe(pipe2_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    switch (fork())
    {
    case 0:
        // child

        if (close(pipe2_fd[0]) == -1)
        {
            perror("close");
            exit(1);
        }

        // produco un risultato
        sleep(2);

        char *buffer = "holaaa";
        int res = write(pipe2_fd[1], buffer, strlen(buffer));

        if (close(pipe2_fd[1]) == -1)
        {
            perror("pipe");
            exit(1);
        }

        exit(0);
        break;
    case -1:
        perror("fork");
        exit(1);
    default:
        if (close(pipe2_fd[1]) == -1)
        {
            perror("close");
            exit(1);
        }

        break;
    }

    char buffer1[20];
    char buffer2[20];

    int res;
    while ((res = read(pipe_fd[0], buffer1, 20)) > 0)
    {
        if (res == -1)
        {
            perror("exit");
            exit(1);
        }
        else if (res == 0)
        {
            break;
        }
    }

    while ((res = read(pipe2_fd[0], buffer2, 20)) > 0)
    {
        if (res == -1)
        {
            perror("exit");
            exit(1);
        }
        else if (res == 0)
        {
            break;
        }
    }

    if (close(pipe_fd[0]) == -1)
    {
        perror("close");
        exit(1);
    }

    if (close(pipe2_fd[0]) == -1)
    {
        perror("close");
        exit(1);
    }

    char result[40];
    strcpy(result, buffer1);
    strcat(result, buffer2);

    printf("%s", result);

    return 0;
}
