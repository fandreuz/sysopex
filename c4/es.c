#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct
{
    char *data;
    unsigned int len;
} packet_t;

int main(int argc, char **argv)
{
    int parent_to_child[2];
    if (pipe(parent_to_child) == -1)
    {
        perror("pipe");
        exit(1);
    }

    int child_to_parent[2];
    if (pipe(child_to_parent) == -1)
    {
        perror("pipe");
        exit(1);
    }

    switch (fork())
    {
        case 0:
            // child

            char buffer[PIPE_BUF];
            int res = read(parent_to_child[0], buffer, PIPE_BUF);
            if(res == -1) {
                perror("read");
                exit(1);
            }

            packet_t *filepack = (packet_t*) buffer;

            printf("file size %d\n", filepack->len);

            res = write(child_to_parent[1], 5, sizeof(int));
            if(res == -1) {
                perror("write");
                exit(1);
            }

            exit(0);
            break;
        case -1:
            perror("fork");
            exit(1);
            break;
        default:
            // parent
            break;
    }

    close(parent_to_child[0]);
    close(child_to_parent[1]);

    char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    char file_buffer[PIPE_BUF];
    int res = read(fd, file_buffer, PIPE_BUF);
    if(res == -1) {
        perror("read");
        exit(1);
    }

    packet_t buffer;
    buffer.len = res;
    buffer.data = file_buffer;

    if(write(parent_to_child[1], &buffer, sizeof(buffer)) == -1) {
        perror("write");
        exit(1);
    }
}
