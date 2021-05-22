#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int *gen_random_array(unsigned int seed, int use_seed, unsigned int number_elements);
int *join_arrays_same_length(int *array1, int *array2, int size);

int main() {
    int *arr1 = gen_random_array(10, 1, 100);
    int *arr2 = gen_random_array(20, 1, 100);

    int* fusion = join_arrays_same_length(arr1, arr2, 100);

    int fd = open("prova.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    char *buffer = calloc(100, sizeof(char));
    for (int i = 0; i < 100; i++)
    {
        int bytes_written = sprintf(buffer, "%d\n", fusion[i]);
        if (bytes_written <= 0) {
            perror("sprintf");
            exit(1);
        }

        bytes_written = write(fd, buffer, bytes_written);
        if (bytes_written <= 0) {
            perror("write");
            exit(1);
        }
    }

    exit(0);
}

int * gen_random_array(unsigned int seed, int use_seed, unsigned int number_elements) {
    if (use_seed != 0) {
        srand(seed);
    }

    int *arr;
    arr = calloc(number_elements, sizeof(int));
    if (arr == NULL) {
        perror("calloc");
        exit(1);
    }

    for (int i = 0; i < number_elements; i++)
    {
        arr[i] = rand();
    }

    return arr;
}

int *join_arrays_same_length(int *array1, int *array2, int size) {
    int *fusion = calloc(size, sizeof(int));
    for (int i = 0; i < size; i++)
    {
        fusion[i] = array1[i] > array2[i] ? array1[i] : array2[i];
    }
    return fusion;
}
