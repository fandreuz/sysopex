#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 10000

int *address;
char *semaphore_name = "/il_mio_semaforo";
sem_t *semaphore;

void processoB()
{
    for (int i = 0; i < N; i++)
    {
        if (sem_wait(semaphore) != 0)
        {
            perror("sem_wait");
            exit(1);
        }

        address[0]++;

        if (address[0] % 10 == 0)
        {
            address[1]++;
        }

        if (sem_post(semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }
    }
}

void processoA()
{
    processoB();
}

int main()
{
    int fd;

    sem_unlink(semaphore_name);
    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 1);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    fd = open("mmap.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    if (ftruncate(fd, 2 * sizeof(int)) != 0)
    {
        perror("ftruncate");
        exit(1);
    }

    address = mmap(NULL, 2 * sizeof(int),
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   fd, 0);
    close(fd);

    if (address == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    for (int i = 0; i < 2; i++)
    {
        int p = fork();

        if (p == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (p == 0)
        {
            if (i == 0)
            {
                processoA();
            }
            else
            {
                processoB();
            }

            exit(0);
        }
        else
        {
            continue;
        }
    }

    while (wait(NULL) != -1) ;

    printf("Cont1: %d\nCont2: %d", address[0], address[1]);

    return 0;
}
