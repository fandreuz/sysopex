#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>

char *semaphore_name = "/il_mio_semaforo";
sem_t *semaphore;

char *file_content;
char *file_dest;

pthread_t t1, t2;

void *thread1_entry_point(void *arg)
{
    char *filename = arg;
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    file_content = (char *) malloc(100 + 1);
    if (file_content == NULL)
    {
        perror("calloc");
        exit(1);
    }

    int current_size = 100;
    int busy_size = 0;

    int res;
    while ((res = read(fd, file_content + busy_size, current_size - busy_size)) != 0)
    {
        if (res == -1)
        {
            perror("read");
            exit(1);
        }

        printf("Read %d bytes\n", res);

        busy_size += res;
        if (busy_size == current_size)
        {
            printf("\nNeed to realloc\n");

            file_content = (char *) realloc(file_content, 2 * current_size + 1);
            if (file_content == NULL)
            {
                perror("realloc");
                exit(1);
            } else {
                current_size *= 2;
            }
        }
    }

    printf("[1]: Ho finito di leggere (res=%d)\n", res);
    printf("%s\n\n", file_content);

    // aspetto che thread2 segnali di aver settato il signal_handler
    if (sem_wait(semaphore) != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    printf("[1] Signal handler di [2] settato!\n");

    if (pthread_kill(t2, SIGUSR1) != 0)
    {
        perror("pthread_kill");
    }

    printf("[1] Ho finito, mando il segnale a [2]\n");

    return NULL;
}

void process_signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("[2] Ho ricevuto il segnale da [1]\n");

        int fd = open(file_dest, O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
        if (fd == -1)
        {
            perror("open");
            exit(1);
        }

        int res = write(fd, file_content, strlen(file_content));
        if (res <= 0)
        {
            perror("write");
            exit(1);
        }

        printf("Done");

        free(file_content);

        exit(0);
    }
    else
    {
        printf("Signal ignored\n");
    }
}

void *thread2_entry_point(void *arg)
{
    file_dest = arg;

    if (signal(SIGUSR1, process_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    if (sem_post(semaphore) != 0)
    {
        perror("sem_post");
        exit(1);
    }

    printf("[2] Ho settato il signal_handler, vado in PAUSA\n");

    pause();

    return NULL;
}

int main(int argc, char **argv)
{
    sem_unlink(semaphore_name);

    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 0);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    int res;

    res = pthread_create(&t1, NULL, thread1_entry_point, argv[1]);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    res = pthread_create(&t2, NULL, thread2_entry_point, argv[2]);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(t1, NULL) != 0)
    {
        perror("pthread_join");
        exit(1);
    }

    if (pthread_join(t2, NULL) != 0)
    {
        perror("pthread_join");
        exit(1);
    }

    return 0;
}
