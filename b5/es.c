#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>

char *semaphore_name = "/semaforo";
sem_t *semaphore;

int *address;

void spawned_process()
{
    printf("VIVO! %d\n", getpid());

    if (sem_wait(semaphore) != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    int counter = address[0];

    printf("counter: %d (%d)\n", counter, getpid());

    if (counter == 20)
    {
        printf("MUOIO FELICE: %d\n", getpid());

        for (int i = 0; i < 19; i++)
        {
            if (sem_post(semaphore) != 0)
            {
                perror("sem_post");
                exit(1);
            }
        }

        exit(0);
    }

    counter++;
    address[0] = counter;

    switch (fork())
    {
    case 0:
        // child
        spawned_process();
        exit(1);
        break;
    case -1:
        perror("fork");
        exit(1);
        break;
    default:
        break;
    }

    if (counter == 20)
    {
        printf("MUOIO FELICE: %d\n", getpid());

        for (int i = 0; i < 19; i++)
        {
            if (sem_post(semaphore) != 0)
            {
                perror("sem_post");
                exit(1);
            }
        }

        exit(0);
    }

    counter++;
    address[0] = counter;

    switch (fork())
    {
    case 0:
        // child
        spawned_process();
        exit(1);
        break;
    case -1:
        perror("fork");
        exit(1);
        break;
    default:
        break;
    }

    if (sem_post(semaphore) != 0)
    {
        perror("sem_post");
        exit(1);
    }

    if (counter == 20)
    {
        printf("MUOIO FELICE: %d\n", getpid());

        for (int i = 0; i < 19; i++)
        {
            if (sem_post(semaphore) != 0)
            {
                perror("sem_post");
                exit(1);
            }
        }

        exit(0);
    }
}

int main()
{
    sem_unlink(semaphore_name);
    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 1);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    int fd = open("mmap.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    int value = 1;
    int res = write(fd, &value, sizeof(int));
    if (res == -1)
    {
        perror("write");
        exit(1);
    }

    address = mmap(NULL, sizeof(int),
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   fd, 0);

    if (address == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    close(fd);

    address[0] = 0;

    spawned_process();

    return 0;
}
