#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

void figlioA();
void figlioB();
void parent_signal_handler();
void childB_signal_handler();

int *address;
int stato;
pid_t b_pid, a_pid;
int parent_terminato = 0;

char *semaphore_name = "/il_mio_semaforo";
sem_t *semaphore;

int main()
{
    sem_unlink(semaphore_name);
    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 0);

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
                   MAP_PRIVATE,
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
    if (signum == SIGUSR1)
    {
        if (kill(getppid(), SIGUSR1) != 0)
        {
            perror("kill");
            exit(1);
        }
    }
    else if (signum == SIGCHLD)
    {
        pid_t p = wait(NULL);
        if (p == b_pid)
        {
            if (kill(a_pid, SIGKILL) != 0)
            {
                perror("kill");
                exit(1);
            }
        }
    }
}

void childB_signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        // inverti valore di stato
        stato = (stato + 1) % 2;
    }
}

void figlioA()
{
    for (int i = 0; i < 100000; i++)
    {
        if (sem_wait(semaphore) != 0)
        {
            perror("sem_wait");
            exit(1);
        }

        printf("A: Cont1=%d\n", address[0]);
        address[0]++;

        if (address[0] % 7 == 0)
        {
            if (kill(getppid(), SIGUSR1) != 0)
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
        printf("B con stato %d\n", stato);

        usleep(100);
    }
}
