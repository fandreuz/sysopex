#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void insertion_sort(int *array, int size);

int main() {
    int fd = open("numeri.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    char line_buffer[100];
    int line_buffer_idx = 0;

    int numbers[1000];
    int numbers_idx = 0;

    int bytes_read;
    while((bytes_read = read(fd, &line_buffer[line_buffer_idx], 1)) == 1) {
        if (line_buffer[line_buffer_idx] == '\n') {
            int result = sscanf(line_buffer, "%d", &numbers[numbers_idx++]);
            if (result <= 0) {
                perror("sscanf");
                exit(1);
            } else {
                for (int i = 0; i <= line_buffer_idx; i++) {
                    line_buffer[i] = 0;
                }
                line_buffer_idx = -1;
            }
        }

        line_buffer_idx++;
    }

    for (int i = 0; i < numbers_idx; i++) {
        printf("%d\n", numbers[i]);
    }

    printf("======== SORT ==========\n");

    insertion_sort(numbers, numbers_idx);

    printf("\n\n\n");

    for (int i = 0; i < numbers_idx; i++) {
        printf("%d\n", numbers[i]);
    }

    exit(0);
}

void insertion_sort(int* array, int size) {
    printf("size: %d\n", size);

    for (int i = 1; i < size; i++) {
        printf("START i, n: %d, %d\n", i, array[i]);
        for (int j = i - 1; j >= 0; j--) {
            if (array[j+1] < array[j]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            } else {
                break;
            }

            printf("i: %d\n", i);
        }

        printf("fine ciclo %d\n", i);
        for (int k = 0; k < i; k++)
        {
            printf("%d\n", array[k]);
        }
    }
}
