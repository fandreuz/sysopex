#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
//versione simple
void controllo(char **buffer, char *parola, int righe)
{
    if ((righe < 0))
    {
        perror("mumero di righe o colonne o parole non valido");
        exit(1);
    }
    int help = 0; //0 parola non trovata
    for (int i = 0; i < righe; i++)
    {
        if (parola == buffer[i])
        {
            help = 1; //1 parola trovata
        }
        if (help == 0)
            printf("non ho trovato la seguente parola %s\n", parola);
    }
}
void lettura(int fd)
{
    char cont_buffer[64]; //ci serve per capire quante righe ci sono
    //64 dimensione massima di una parola
    memset(cont_buffer, 0, 64);
    long int n_righe = 1;
    int i = 0;
    while (1)
    { //contiamo il numero di righe
        int res = read(fd, &cont_buffer[i], 1);
        if (res == -1)
        {
            perror("errore con read()");
            exit(1);
        }
        if (cont_buffer[i] == EOF)
            break;
        if (cont_buffer[i] == '\n')
        {
            n_righe++;
            //printf(" riga:%d\n",n_righe);
            memset(cont_buffer, 0, 64);
            i = 0;
        }
        else
            i++;
    }
    //printf("ci sono %ld righe\n",n_righe);
    i = 0;
    int j = 0;
    char buffer[n_righe][64]; //usiamo questa matrice come array di stringhe
    lseek(fd, 0, SEEK_SET);
    while ((i < 64) && (j < n_righe))
    {
        int res = read(fd, &buffer[j][i], 1);
        if (res == -1)
        {
            perror("errore con read() 2");
            exit(1);
        }
        if (res == 0)
            break;
        else if (buffer[j][i] == '\n')
        {
            j++;
            i = 0;
        }
        else
            i++;
    }
    char **buf = malloc(n_righe * sizeof(char *));
    if (buf == NULL)
    {
        perror("errore con buf");
        exit(1);
    }
    for (i = 0; i < n_righe; i++)
    {
        for (j = 0; j < 64; j++)
        {
            buf[i] = buffer[i];
            //printf("%s\n",buf[i]);
        }
    }
    char *token;
    char s[] = " .,;";
    char *words_to_check[] = {
        "At", "Bell", "Laborotories",
        "UNIX", "systems", "privide",
        "more", "timesharing", "ports",
        "than", "all", "other", "systems",
        "combined", NULL};
    // token = strtok(words_to_check, s);
    for (int k = 0; k < 14; k++)
    { //14 è il numero di parole in word_to_check
        int pid = fork();
        // token = strtok(NULL, s);
        if (token != NULL)
        {
            switch (pid)
            {
            case -1:
                perror("errore con fork()");
                exit(1);
                break;
            case 0: //processo figlio
                controllo(buf, words_to_check[k], n_righe);
                break;
            default:
                sleep(10);
                if (wait(NULL) == -1)
                {
                    perror("wait");
                    //exit(1);
                }
            }
        }
    }
    if (close(fd) == -1)
    {
        perror("errore con close()");
        exit(1);
    }
    free(buf);
}
int main(int argc, char **argv)
{
    char *file = "american-english";
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("errore con open()");
        exit(1);
    }
    lettura(fd);
    return 0;
}
