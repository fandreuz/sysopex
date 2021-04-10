#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int check_occurrences(char *dataset, char *stringa);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Manca l'input");
        exit(1);
    }

    char buffer[10000];
    int result = read(0, buffer, 10000);
    if (result <= 0) {
        perror("read");
        exit(1);
    }

    int occ = check_occurrences(buffer, argv[1]);
    printf("%d\n", occ);
}

int check_occurrences(char* dataset, char* stringa) {
    printf("%s\n\n%s\n\n", dataset, stringa);

    int occurrences = 0;
    char *first_occ;
    while ((first_occ = strstr(dataset, stringa)) != NULL)
    {
        occurrences++;
        dataset = first_occ + strlen(stringa);
    }

    return occurrences;
}
