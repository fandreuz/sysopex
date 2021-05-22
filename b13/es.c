#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

char *semaphore_name = "/il_mio_semaforo";
sem_t *semaphore;

char *semaphore2_name = "/il_mio_semaforo2";
sem_t *semaphore2;

typedef struct
{
    long *x;
    long *y;
    int array_size;
    int result_ok;
    int id;
} thread_parameter_t;

void *thread_entry_point(void *arg)
{
    printf("[thread] ok! dormo!\n");

    if (sem_wait(semaphore) != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    thread_parameter_t *param = arg;

    for (int i = 0; i < param->array_size; i++)
    {
        param->y[i] = pow((double)param->x[i], (double)4) - pow((double)param->x[i], (double)3) + pow((double)param->x[i], (double)2);
    }

    param->result_ok = 1;

    printf("io ho finito %d\n", param->id);
    if (sem_post(semaphore2) != 0)
    {
        perror("sem_post");
        exit(1);
    }

    return NULL;
}

int main()
{
    sem_unlink(semaphore_name);
    sem_unlink(semaphore2_name);

    semaphore = sem_open(semaphore_name, O_CREAT, 0600, 0);
    semaphore2 = sem_open(semaphore2_name, O_CREAT, 0600, 0);

    if (semaphore == SEM_FAILED || semaphore2 == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    pthread_t ts[16];
    thread_parameter_t params[16];

    for (int i = 0; i < 16; i++)
    {
        params[i].result_ok = 0;
        params[i].x = calloc(i + 10, sizeof(long));
        for (int j = 0; j < i + 10; j++)
        {
            params[i].x[j] = i + j;
        }
        params[i].id = i;
        params[i].array_size = i + 10;
        params[i].y = calloc(i + 10, sizeof(long));

        int res = pthread_create(&ts[i],
                                 NULL,
                                 thread_entry_point,
                                 &params[i]);

        if (res != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < 16; i++)
    {
        if (sem_post(semaphore) != 0)
        {
            perror("sem_post");
            exit(1);
        }
    }

    for (int i = 0; i < 16; i++)
    {
        if (sem_wait(semaphore2) != 0)
        {
            perror("sem_wait");
            exit(1);
        }

        int index;
        for (int j = 0; j < 16; j++)
        {
            if (params[j].result_ok == 1)
            {
                index = j;
                params[j].result_ok = 0;
                break;
            }
        }

        thread_parameter_t param = params[index];
        printf("ricevuto risultato da %d\n", index);
        for (int j = 0; j < param.array_size; j++)
        {
            printf("\t%ld\n", param.y[j]);
        }

        free(param.x);
        free(param.y);
    }
}
