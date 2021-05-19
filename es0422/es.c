#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#define N 10

int fds[10];
int num_cicli[N];

int main()
{
    int text = 0;

    int i;
    for (i = 0; i < N; i++)
    {
        char filename[100];
        sprintf(filename, "contatore%d.txt", i);

        fds[i] = open(filename, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
        if (fds[i] == -1)
        {
            perror("open");
            exit(1);
        }

        int res = write(fds[i], &text, sizeof(int));
        if (res != sizeof(int))
        {
            perror("write");
            exit(1);
        }
    }

    srand(1234);

    for (i = 0; i < N; i++)
    {
        num_cicli[i] = rand() % 20;
        printf("num_cicli[%d]: %d\n", i, num_cicli[i]);
    }

    int pipe_fd[2];
    if (pipe2(pipe_fd, O_NONBLOCK) == -1)
    {
        perror("pipe2");
        exit(1);
    }

    for (i = 0; i < N; i++)
    {
        switch (fork())
        {
        case -1:
            perror("fork");
            exit(1);
        case 0:
            // padre
            printf("padre\n");
            break;
        default:
            printf("figlio\n");
            // figlio
            if (close(pipe_fd[0]) == -1)
            {
                perror("close");
                exit(1);
            }

            int j;
            for (j = 0; j < num_cicli[i]; j++)
            {
                if (flock(fds[i], LOCK_EX) == -1)
                {
                    perror("flock");
                    exit(1);
                }

                int read_result = -1;

                if (lseek(fds[i], SEEK_SET, 0) != 0)
                {
                    perror("lseek");
                    exit(1);
                }

                int res = read(fds[i], &read_result, sizeof(int));
                if (res == -1)
                {
                    perror("read");
                    exit(1);
                }

                if (lseek(fds[i], SEEK_SET, 0) != 0)
                {
                    perror("lseek");
                    exit(1);
                }

                printf("letto result for %d: %d (%d)\n", j, read_result, res);
                read_result++;
                printf("new read result for %d: %d\n", j, read_result);

                res = write(fds[i], &read_result, sizeof(int));
                if (res == -1)
                {
                    perror("write");
                    exit(1);
                }

                if (flock(fds[i], LOCK_UN) == -1)
                {
                    perror("flock");
                    exit(1);
                }

                sleep(1);
            }

            if (close(pipe_fd[1]) == -1)
            {
                perror("close");
                exit(1);
            }

            sleep(num_cicli[i] / 4);

            exit(0);
        }
    }

    if (close(pipe_fd[1]) == -1)
    {
        perror("close");
        exit(1);
    }

    int dummy;

    while (1)
    {
        int res = read(pipe_fd[0], &dummy, sizeof(int));
        if (res == -1)
        {
            if (errno == EAGAIN)
            {
                for (i = 0; i < N; i++)
                {
                    if (flock(fds[i], LOCK_EX) == -1)
                    {
                        perror("flock");
                        exit(1);
                    }

                    int read_result;

                    if (lseek(fds[i], SEEK_SET, 0) != 0)
                    {
                        perror("lseek");
                        exit(1);
                    }

                    int res = read(fds[i], &read_result, sizeof(int));
                    if (res == -1)
                    {
                        perror("read");
                        exit(1);
                    }

                    printf("contatore %d = %d\n", i, read_result);
                }

                sleep(1);
            }
            else
            {
                perror("read");
                exit(1);
            }
        }
        else
        {
            printf("EOF!!!!\n");
            break;
        }
    }

    printf("-----------------------");

    while (wait(NULL) != -1) {}

    printf("ok");

    return 0;
}
