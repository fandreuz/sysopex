#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#define FIBO_N 10

int process_fibo_index;
int *fibo_results;
int *barrier_count;
char *fibo_results_semaphore_name = "/fibo_sem";
char *tornello_semaphore_name = "/tornello_sem";
char *barrier_count_semaphore_name = "/barrier_count_sem";
sem_t *fibo_results_semaphore;
sem_t *tornello_semaphore;
sem_t *barrier_count_semaphore;

pid_t pids[FIBO_N];

void process_signal_handler(int signum)
{
    printf("[%d] ho ricevuto qualcosa\n", process_fibo_index);
    if (signum == SIGUSR1 && process_fibo_index > 2)
    {
        printf("[%d] un SIGUSER\n", process_fibo_index);
        if (sem_wait(fibo_results_semaphore) != 0)
        {
            perror("sem_wait");
            exit(1);
        }

        fibo_results[process_fibo_index - 1] = fibo_results[process_fibo_index - 3] + fibo_results[process_fibo_index - 2];

        if (sem_post(fibo_results_semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }

        if (kill(pids[process_fibo_index], SIGUSR1) == -1)
        {
            perror("kill");
            exit(1);
        }
    }
}

void fibo_process(int n)
{
    process_fibo_index = n;

    printf("[%d] VIVO!\n", n);

    if (signal(SIGUSR1, process_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    if (sem_wait(barrier_count_semaphore) != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    barrier_count[0]++;

    int res;
    if ((res = sem_post(barrier_count_semaphore)) != 0)
    {
        // se sono stato interrotto
        if (errno != EINTR)
        {
            perror("sem_post");
            exit(1);
        }
    }

    if (barrier_count[0] == FIBO_N)
    {
        if (sem_post(tornello_semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }
    }

    if (n == 1)
    {
        if (sem_wait(tornello_semaphore) != 0)
        {
            perror("sem_wait");
            exit(1);
        }
        if (sem_post(tornello_semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }

        // da qui in poi hanno registrato tutti il processatore di segnali
        if (n == 1 || n == 2)
        {
            fibo_results[n - 1] = 1;

            // we want to do this only once
            if (n == 1)
            {
                if (sem_post(fibo_results_semaphore) != 0)
                {
                    perror("sem_post");
                    exit(1);
                }
            }

            if (n == 2 && kill(pids[n], SIGUSR1) == -1)
            {
                perror("kill");
                exit(1);
            }
        }
    }
    else
    {
        pause();
    }
}

int main()
{
    sem_unlink(fibo_results_semaphore_name);
    // quanto il mio semaforo è chiuso nessuno può scrivere/leggere dalla
    // mmap con i risultati
    fibo_results_semaphore = sem_open(fibo_results_semaphore_name, O_CREAT, 0600, 0);
    if (fibo_results_semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_unlink(tornello_semaphore_name);
    tornello_semaphore = sem_open(tornello_semaphore_name, O_CREAT, 0600, 0);
    if (tornello_semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_unlink(barrier_count_semaphore_name);
    barrier_count_semaphore = sem_open(barrier_count_semaphore_name, O_CREAT, 0600, 1);
    if (barrier_count_semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    int result_fd, barrier_count_fd;

    result_fd = open("results_mmap.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (result_fd == -1)
    {
        perror("open");
        exit(1);
    }

    if (ftruncate(result_fd, sizeof(int) * FIBO_N) != 0)
    {
        perror("ftruncate");
        exit(1);
    }

    barrier_count_fd = open("barrier_count_mmap.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (barrier_count_fd == -1)
    {
        perror("open");
        exit(1);
    }

    if (ftruncate(barrier_count_fd, sizeof(int) * FIBO_N) != 0)
    {
        perror("ftruncate");
        exit(1);
    }

    fibo_results = mmap(NULL, sizeof(int) * FIBO_N,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        result_fd, 0);

    if (fibo_results == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    barrier_count = mmap(NULL, sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         barrier_count_fd, 0);

    if (barrier_count == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    for (int i = 0; i < FIBO_N; i++)
    {
        pid_t p;
        switch ((p = fork()))
        {
        case -1:
            perror("fork");
            exit(1);
        case 0:
            printf("figlio!\n");
            // processo figlio
            fibo_process(i + 1);
            exit(0);
        default:
            pids[i] = p;
            break;
        }
    }

    for (int i = 0; i < FIBO_N; i++)
    {
        if (wait(NULL) == -1)
        {
            perror("wait");
            exit(1);
        }
    }

    for (int i = 0; i < FIBO_N; i++)
    {
        printf("%d : %d\n", i, fibo_results[i]);
    }

    return 0;
}
