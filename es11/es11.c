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
    float averages_sum = 0;

    for (int i = 0; i < numero_cicli; i++)
    {
        getloadavg(loadavg, 3);

        averages_sum += loadavg[0];

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

    int pid = fork();
    if (pid == 0) {
        // figlio
        float result = averages_sum / numero_cicli;
        printf("Average: %f\n", result);

        int result_fd = open("result.txt", O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        if (result_fd == -1) {
            perror("open");
            exit(1);
        }

        char result_buffer[100];
        int sres = sprintf(result_buffer, "%f\n", result);
        if (sres < 1) {
            perror("sprintf");
            exit(1);
        }

        int wres = write(result_fd, result_buffer, 100);
        if (wres <= 0) {
            perror("write");
            exit(1);
        }
    }
    else if (pid > 0) {
        // padre
        int wres = wait(&pid);
        if (wres == -1) {
            perror("wait");
            exit(1);
        }

        int result_fd = open("result.txt", O_RDONLY);
        if (result_fd == -1) {
            perror("open");
            exit(1);
        }

        char buffer[100];
        int rres = read(result_fd, buffer, 100);
        if (rres <= 0) {
            perror("read");
            exit(1);
        }

        float written_average;
        int sres = sscanf(buffer, "%f", &written_average);
        if (sres <= 0) {
            perror("sscanf");
            exit(1);
        }

        printf("child ha scritto %f\n", written_average);
    }
    else
    {
        perror("fork");
        exit(1);
    }

    return 0;
}
