#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {

    if (argc != 3)
    {
        printf("Il numero di parametri è errato (sorgente, destinazione)\n");
        exit(1);
    }

    int source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1) {
        perror("open");
        exit(1);
    }

    int dest_fd = open(argv[2], O_CREAT | O_TRUNC | O_RDWR,  S_IRUSR | S_IWUSR);
    if (dest_fd == -1) {
        perror("open");
        exit(1);
    }

    char *buffer = calloc(100, sizeof(char));
    if (buffer == NULL) {
        perror("calloc");
        exit(1);
    }

    int read_result;
    while ((read_result = read(source_fd, buffer, 100)) != 0) {
        if (read_result == -1) {
            perror("read");
            exit(1);
        } else {
            int write_result = write(dest_fd, buffer, read_result);
            if (write_result == -1) {
                perror("write");
                exit(1);
            } else if (write_result != read_result) {
                printf("No space");
                exit(1);
            }
        }
    }

    free(buffer);
    exit(0);
}
