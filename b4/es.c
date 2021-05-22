#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *semaphore_name_padre = "/semaforo_padre";
char *semaphore_name_figlio = "/semaforo_figlio";
sem_t *semaphore_padre;
sem_t *semaphore_figlio;

int counter = 0;

int main()
{
    sem_unlink(semaphore_name_padre);
    semaphore_padre = sem_open(semaphore_name_padre, O_CREAT, 0600, 0);

    sem_unlink(semaphore_name_figlio);
    semaphore_figlio = sem_open(semaphore_name_figlio, O_CREAT, 0600, 1);

    int fd = open("file.txt", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    switch (fork())
    {
        case 0:
            while (counter < 100)
            {
                if (sem_wait(semaphore_padre) != 0)
                {
                    perror("sem_wait");
                    exit(1);
                }

                char testo[] = "[figlio]\n";
                write(fd, testo, strlen(testo));

                if (sem_post(semaphore_figlio) != 0)
                {
                    perror("sem_post");
                    exit(1);
                }

                printf("figlio %d\n", counter);

                if (counter >= 100) {
                    exit(0);
                }
            }

            // parent
            break;
        case -1:
            perror("fork");
            exit(1);
        default:
            // padre

            while (counter < 100)
            {
                if (sem_wait(semaphore_figlio) != 0)
                {
                    perror("sem_wait");
                    exit(1);
                }

                char testo[20];
                sprintf(testo, "[padre]: %d\n", counter++);
                write(fd, testo, strlen(testo));

                printf("padre %d\n", counter);

                if (sem_post(semaphore_padre) != 0)
                {
                    perror("sem_post");
                    exit(1);
                }
            }

            break;
    }

    return 0;
}
