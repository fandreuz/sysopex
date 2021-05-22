#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

typedef struct
{
    int x;
} thread_parameter_t;

typedef struct
{
    int y;
} thread_result_t;

void *thread1_func(void *arg)
{
    thread_parameter_t *param = arg;

    thread_result_t *result = malloc(sizeof(thread_result_t));
    if (result == NULL)
    {
        perror("malloc");
        exit(1);
    }

    result->y = pow((double)param->x, 4);
    return result;
}

void *thread2_func(void *arg)
{
    thread_parameter_t *param = arg;

    thread_result_t *result = malloc(sizeof(thread_result_t));
    if (result == NULL)
    {
        perror("malloc");
        exit(1);
    }

    result->y = pow((double)param->x, 3);
    return result;
}

void *thread3_func(void *arg)
{
    thread_parameter_t *param = arg;

    thread_result_t *result = malloc(sizeof(thread_result_t));
    if (result == NULL)
    {
        perror("malloc");
        exit(1);
    }

    result->y = pow((double)param->x, 2);
    return result;
}

int calcola(int x)
{
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;

    void *thread1_result;
    void *thread2_result;
    void *thread3_result;

    thread_parameter_t param;
    param.x = x;

    // creazione thread
    int res = pthread_create(&t1,
                             NULL,
                             thread1_func,
                             &param);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    res = pthread_create(&t2,
                         NULL,
                         thread2_func,
                         &param);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    res = pthread_create(&t3,
                         NULL,
                         thread3_func,
                         &param);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    // attendo i risultati

    res = pthread_join(t1, &thread1_result);
    if (res != 0)
    {
        perror("pthread_join");
        exit(1);
    }

    res = pthread_join(t2, &thread2_result);
    if (res != 0)
    {
        perror("pthread_join");
        exit(1);
    }

    res = pthread_join(t3, &thread3_result);
    if (res != 0)
    {
        perror("pthread_join");
        exit(1);
    }

    thread_result_t *t1r = (thread_result_t *)thread1_result;
    thread_result_t *t2r = (thread_result_t *)thread2_result;
    thread_result_t *t3r = (thread_result_t *)thread3_result;

    return t1r->y - t2r->y + t3r->y;
}

int main() {
    printf("%d", calcola(10000));
}
