#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void insertion_sort(double *array, int size);
double *insert_into_array(double *array, int array_len, double val, int insert_position);

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
    double averages[numero_cicli];
    double averages_sum = 0;

    for (int i = 0; i < numero_cicli; i++)
    {
        getloadavg(loadavg, 3);

        averages[i] = loadavg[0];
        averages_sum += loadavg[0];

        int printed_number = sprintf(buffer, "%lf\n", loadavg[0]);
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
        double result = averages_sum / (double) numero_cicli;
        printf("Average: %lf\n", result);

        int result_fd = open("result.txt", O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        if (result_fd == -1) {
            perror("open");
            exit(1);
        }

        char result_buffer[100];
        int sres = sprintf(result_buffer, "%lf\n", result);
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

        double *new_array;

        // ordina i valori
        insertion_sort(averages, numero_cicli);
        for (int i = 0; i < numero_cicli; i++) {
            if (averages[i] > written_average) {
                new_array = insert_into_array(averages, numero_cicli, written_average, i);
                break;
            }

            if (i == numero_cicli - 1) {
                new_array = insert_into_array(averages, numero_cicli, written_average, numero_cicli);
            }
        }

        printf(" =============== nuovo array\n");
        for (int i = 0; i < numero_cicli + 1; i++)
        {
            printf("new_array[%d] = %lf\n", i, new_array[i]);
        }
    }
    else
    {
        perror("fork");
        exit(1);
    }

    return 0;
}

void insertion_sort(double* array, int size) {
    printf("\nordina:\n");
    for (int i = 0; i < size; i++) {
        printf("array[%d] = %lf\n", i, array[i]);
    }

    for (int i = 1; i < size; i++) {
        for (int j = i - 1; j >= 0; j--) {
            if (array[j+1] < array[j]) {
                double temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            } else {
                break;
            }
        }
    }
}

double * insert_into_array(double * array, int array_len, double val, int insert_position) {
    if (insert_position < 0 || insert_position > array_len) {
        return NULL;
    }

    double *new_array = calloc(array_len+1, sizeof(double));
    for (int i = 0, old_array_idx = 0; i < array_len + 1; i++) {
        if (i == insert_position) {
            new_array[i] = val;
        } else {
            new_array[i] = array[old_array_idx++];
        }
    }

    return new_array;
}
