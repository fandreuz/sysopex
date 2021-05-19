#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <semaphore.h>
#include <pthread.h>

#define N 1000
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
char *semaphore_name = "/il_mio_semaforo";
sem_t *semaphore; //definisco il semaforo
int fd;
int var_cond;

void child_process()
{
    int seed = 1000;
    srand(seed);
    int strlen;
    char buffer[12];
    for (int i = 0; i < N; i++)
    {
        lseek(fd, 0, SEEK_SET);
        if (pthread_mutex_lock(&mtx) != 0)
        {
            perror("pthread_mutex_lock");
            exit(1);
        }
        var_cond = rand();
        strlen = sprintf(buffer, "%d", var_cond);
        if (write(fd, buffer, strlen) == -1)
        {
            perror("write()");
            exit(1);
        }
        printf("var_cond:%d\n", var_cond);
        if (pthread_mutex_unlock(&mtx) != 0)
        {
            perror("pthread_mutex_lock");
            exit(1);
        }
        if (sem_post(semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }
        sleep(1);
    }
}

void parent_process()
{
    int buffer_len = 12;
    char buffer[12];
    while (1)
    {
        lseek(fd, 0, SEEK_SET);
        if (sem_wait(semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }
        if (pthread_mutex_lock(&mtx) != 0)
        {
            perror("pthread_mutex_lock");
            exit(1);
        }
        if (read(fd, &buffer, buffer_len) == -1)
        {
            perror("read()");
            exit(1);
        }
        printf("valore buffer:%s\n", buffer);
        sscanf(buffer, "%d", &var_cond);
        printf("valore variabile condivisa:%d\n", var_cond);
        if (pthread_mutex_unlock(&mtx) != 0)
        {
            perror("pthread_mutex_lock");
            exit(1);
        }
    }
}

int main()
{
    sem_unlink(semaphore_name); //cancello se esiste già
    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 0);
    char *file = "prova.txt";
    fd = open(file, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    switch (fork())
    {
    case -1:
        perror("fork()");
        exit(1);
    case 0:
        child_process();
        exit(0);
    default:
        parent_process();
    }
    return 0;
}
