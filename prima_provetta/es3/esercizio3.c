#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int * gen_random_array(unsigned int seed, int use_seed, unsigned int number_elements);

int main() {
    printf("hello");

    int use_seed;
    int result = scanf("%d\n", &use_seed);
    if (result != 1) {
        perror("scanf");
        exit(1);
    }

    int seed = 0;
    if (use_seed)
    {
        int result2 = scanf("%d\n", &seed);
        if (result2 != 1) {
            perror("scanf");
            exit(1);
        }
    }

    int number;
    int result3 = scanf("%d\n", &number);
    if (result3 != 1) {
        perror("scanf");
        exit(1);
    }

    int* arr;
    arr = gen_random_array(use_seed, seed, number);

    for (int i = 0; i < number; i++) {
        printf("%d -> %d\n", i, arr[i]);
    }

    free(arr);

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
