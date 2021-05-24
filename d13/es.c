#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void figlioA();
void figlioB();
void parent_signal_handler();
void childB_signal_handler();

int *address;
int stato;
pid_t b_pid, a_pid, parent_id;
int parent_terminato = 0;

char *semaphore_name = "/il_mio_semaforo";
sem_t *semaphore;

int main()
{
    parent_id = getpid();

    sem_unlink(semaphore_name);
    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 1);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    int fd = open("file.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    if (ftruncate(fd, sizeof(int)) != 0)
    {
        perror("ftruncate");
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

    address[0] = 0;

    if (signal(SIGUSR1, parent_signal_handler) == SIG_ERR || signal(SIGCHLD, parent_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    switch ((a_pid = fork()))
    {
    case 0:
        figlioA(&address);
        exit(0);
        break;
    case -1:
        perror("fork");
        exit(1);
    default:
        // padre
        break;
    }

    switch ((b_pid = fork()))
    {
    case 0:
        figlioB(&address);
        exit(0);
        break;
    case -1:
        perror("fork");
        exit(1);
    default:
        // padre
        break;
    }

    while (parent_terminato != 1)
    {
        pause();
    }

    printf("Cont1: %d\n", address[0]);

    return 0;
}

void parent_signal_handler(int signum)
{
    printf("[padre]: ho ricevuto qualocsa\n");
    if (signum == SIGUSR1)
    {
        if (kill(b_pid, SIGUSR1) != 0)
        {
            perror("kill");
            exit(1);
        }
    }
    else if (signum == SIGCHLD)
    {
        printf("SIGCHIL!\n");
        pid_t p = wait(NULL);
        printf("%d\n", p);
        if (p == a_pid)
        {
            printf("è morto A!\n");
            if (kill(b_pid, SIGKILL) != 0)
            {
                perror("kill");
                exit(1);
            }

            parent_terminato = 1;
        }
    }
}

void childB_signal_handler(int signum)
{
    //printf("B: ho ricevuto qualcosa!\n");
    if (signum == SIGUSR1)
    {
        //printf("SIGUSR1\n");
        // inverti valore di stato
        stato = (stato + 1) % 2;
    }
}

void figlioA()
{
    for (int i = 0; i < 10000; i++)
    {
        if (i % 100 == 0)
        {
            printf("A: %d\n", i);
        }
        //printf("inizio loop A\n");

        if (sem_wait(semaphore) != 0)
        {
            perror("sem_wait");
            exit(1);
        }

        //printf("A: Cont1=%d\n", address[0]);
        address[0]++;

        printf("[A]: cont=%d\n", address[0]);

        if (address[0] % 7 == 0)
        {
            //printf("Mando un segnale a B\n");
            if (kill(parent_id, SIGUSR1) != 0)
            {
                perror("kill");
                exit(1);
            }
        }

        if (sem_post(semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }

        usleep(1000);
    }

    printf("A: muoio\n");
}

void figlioB()
{
    stato = 0;

    if (signal(SIGUSR1, childB_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    while (1)
    {
        //printf("B con stato %d\n", stato);

        int res;
        while (1)
        {
            if ((res = sem_wait(semaphore)) != 0 && errno != EINTR)
            {
                perror("sem_wait");
                exit(1);
            }

            if (res != -1)
            {
                break;
            }
        }

        if (stato)
        {
            address[0]++;
            printf("[B]: incremento!\n");
        }
        else
        {
            address[0]--;
            printf("[B]: decremento!\n");
        }

        if (sem_post(semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }

        usleep(100);
    }
}
