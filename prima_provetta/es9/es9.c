#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef char * char_p;

struct risultato_ricerca {
    int numero_occorrenze; // dimensione del vettore posizione_occorrenza
    char_p * posizione_occorrenza; // vettore delle occorrenze; ogni occorrenza è riportata nel vettore
    // posizione_occorrenza è un vettore di char_p (o char *)

};

struct risultato_ricerca trova_occorrenze(char * str, char * substr);

int main(int argc, char **argv) {
    char buffer[10000];
    int read_result = read(0, buffer, 10000);
    if (read_result <= 0) {
        perror("read");
        exit(1);
    }

    struct risultato_ricerca result = trova_occorrenze(buffer, argv[1]);

    printf("trovate %d occorrenze di %s\n", result.numero_occorrenze, argv[1]);
    for (int i = 0; i < result.numero_occorrenze; i++) {
        printf("- alla posizione %ld;\n", result.posizione_occorrenza[i] - buffer);
    }

    free(result.posizione_occorrenza);

    return 0;
}

struct risultato_ricerca trova_occorrenze(char * str, char * substr) {
    struct risultato_ricerca result;

    result.numero_occorrenze = 0;
    result.posizione_occorrenza = calloc(100, sizeof(char_p));

    char *res;
    while ((result.posizione_occorrenza[result.numero_occorrenze] = strstr(str, substr)) != NULL)
    {
        str = result.posizione_occorrenza[result.numero_occorrenze] + strlen(substr);
        result.numero_occorrenze++;
    }
    return result;
}
