#include <stdlib.h>
#include <stdio.h>

int *insert_into_array(int *array, int array_len, int val, int insert_position);

int main() {
    int arr[] = {0, 1, 2, 5, 6, 7};

    int *new_arr = insert_into_array(arr, 6, 340, -1);
    if (new_arr == NULL) {
        printf("ok");
    } else {
        printf("error");
    }
    printf("\n\n");

    free(new_arr);

    int *new_arr2 = insert_into_array(arr, 6, 340, 20);
    if (new_arr == NULL) {
        printf("ok");
    } else {
        printf("error");
    }
    printf("\n\n");

    free(new_arr2);

    int *new_arr3 = insert_into_array(arr, 6, 340, 5);
    for (int i = 0; i < 7; i++) {
        printf("new_arr[%d] = %d\n", i, new_arr3[i]);
    }
    printf("\n\n");

    free(new_arr3);

    int *new_arr4 = insert_into_array(arr, 6, 340, 2);
    for (int i = 0; i < 7; i++) {
        printf("new_arr[%d] = %d\n", i, new_arr4[i]);
    }
    printf("\n\n");

    free(new_arr4);
}

int * insert_into_array(int * array, int array_len, int val, int insert_position) {
    if (insert_position < 0 || insert_position > array_len) {
        return NULL;
    }

    int *new_array = calloc(array_len+1, sizeof(int));
    for (int i = 0, old_array_idx = 0; i < array_len + 1; i++) {
        if (i == insert_position) {
            new_array[i] = val;
        } else {
            new_array[i] = array[old_array_idx++];
        }
    }

    return new_array;
}
