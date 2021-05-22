#include <stdlib.h>
#include <stdio.h>

int *join_arrays_same_length(int *array1, int *array2, int size);

int main() {
    int a1[] = {0, 1, 2, 3, 4};
    int a2[] = {4, 3, 2, 1, 0};

    int *a3 = join_arrays_same_length(a1, a2, 5);
    for (int i = 0; i < 5; i++) {
        printf("%d\n", a3[i]);
    }

    free(a3);

    exit(0);
}

int *join_arrays_same_length(int *array1, int *array2, int size) {
    int *fusion = calloc(size, sizeof(int));
    for (int i = 0; i < size; i++)
    {
        fusion[i] = array1[i] > array2[i] ? array1[i] : array2[i];
    }
    return fusion;
}
