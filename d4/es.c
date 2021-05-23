#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define N 100

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void handler(int signum) {
    printf("ho ricevuto qualcosa!\n");
    if (signum == SIGCHLD)
    {
        printf("ricevuto segnale di terminazione\n");
        exit(0);
    }
}

int main()
{
    char *semaphore_name = "/il_mio_semaforo";
    sem_t *semaphore;

    sem_unlink(semaphore_name);

    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 0);

    if (semaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    int fd = open("file.txt", O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    if (ftruncate(fd, sizeof(int)) != 0) {
        perror("ftruncate");
        exit(1);
    }

    int *address = mmap(NULL, sizeof(int),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        fd, 0);

    switch (fork())
    {
    case 0:
        // child
        srand(1000);

        for (int i = 0; i < N; i++)
        {
            if (pthread_mutex_lock(&mtx) != 0)
            {
                exit(1);
            }

            int g = rand();
            address[0] = g;
            printf("Generato %d\n", g);

            if (pthread_mutex_unlock(&mtx) != 0)
            {
                perror("pthread_mutex_unlock");
                exit(1);
            }

            if (sem_post(semaphore) != 0)
            {
                perror("sem_post");
                exit(1);
            }

            usleep(10 * 1000);
        }

        //kill(0, SIGCHLD);

        exit(0);
        break;
    case -1:
        perror("fork");
        exit(1);
    default:
        // parent

        if(signal(SIGCHLD, handler) == SIG_ERR) {
            perror("signal");
            exit(1);
        }

        while (1)
        {
            if (sem_wait(semaphore) != 0)
            {
                perror("sem_wait");
                exit(1);
            }

            if (pthread_mutex_lock(&mtx) != 0)
            {
                perror("pthread_mutex_lock");
                exit(1);
            }

            printf("Letto: %d\n", address[0]);

            if (pthread_mutex_unlock(&mtx) != 0)
            {
                perror("pthread_mutex_unlock");
                exit(1);
            }
        }

        break;
    }
}
