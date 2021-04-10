#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        argv[1] = "10";
        argv[2] = "5";
    }

    int numero_cicli;
    int intervallo;

    int res1 = sscanf(argv[1], "%d", &numero_cicli);
    if (res1 != 1) {
        perror("sscanf");
        exit(1);
    }

    int res2 = sscanf(argv[2], "%d", &intervallo);
    if (res2 != 1) {
        perror("sscanf");
        exit(1);
    }

    int fd = open("log.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    char buffer[100];
    double loadavg[3];
    for (int i = 0; i < numero_cicli; i++)
    {
        getloadavg(loadavg, 3);

        int printed_number = sprintf(buffer, "%f\n", loadavg[0]);
        if (printed_number <= 0) {
            perror("sprintf");
            exit(1);
        }

        int write_result = write(fd, buffer, printed_number);
        if (write_result <= 0) {
            perror("write");
            exit(1);
        }

        printf("%d\n", i);
        sleep(intervallo);
    }

    return 0;
}
