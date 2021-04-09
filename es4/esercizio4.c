#include <stdlib.h>
#include <stdio.h>

int find_max_even_positions(int *array, int size);

int main() {
    int arr[] = {73373282, 344, 57, 233, 1334, 132, 324534, 33211, 12321423, 453, 132};
    printf("%d\n", find_max_even_positions(arr, 11));
    exit(0);
}

int find_max_even_positions(int *array, int size) {
    if (size == 0) {
        printf("size is 0");
        return -1;
    }
    else
    {
        int idx = 0;
        for (int i = 2; i < size; i += 2)
        {
            if (array[idx] < array[i]) {
                idx = i;
            }
        }

        return idx;
    }
}
