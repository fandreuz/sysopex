#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *filename;
char *to_search;

void search(int x, int K, int M)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    int file_size = lseek(fd, 0L, SEEK_END);
    if (file_size == -1)
    {
        perror("lseek");
        exit(1);
    }
    printf("file size %d\n", file_size);

    int size = (int)(file_size / M) + K;
    int offset = (int)(file_size / M * x);
    printf("process %d has size %d and offset %d\n", x, size, offset);

    if (lseek(fd, offset, SEEK_SET) != offset)
    {
        perror("lseek");
        exit(1);
    }

    char buffer[size + 1];
    int res = read(fd, buffer, size);
    if (res == -1)
    {
        perror("read");
        exit(1);
    }

    char * trovato = strstr(buffer, to_search);
    if (trovato != NULL)
    {
        printf("TROVATO! %ld\n", trovato - buffer + offset);
    }
}

int main(int argc, char **argv)
{
    filename = argv[1];
    int M = atoi(argv[2]);
    to_search = argv[3];

    for (int i = 0; i < M; i++)
    {
        switch (fork())
        {
        case 0:
            search(i, strlen(to_search) - 1, M);
            exit(0);
            break;
        case -1:
            perror("fork");
            exit(1);
        default:
            continue;
        }
    }

    printf("tutti i processi sono stati creati\n");

    if (wait(NULL) == -1)
    {
        perror("wait");
        exit(1);
    }

    return 0;
}
