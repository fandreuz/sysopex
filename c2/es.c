#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define N 1024

char buffer[32];
int cookie_counter = 0;

pthread_t t1;
pthread_t t2;

void signal1_handler(int signum)
{
    printf("ricevuto (1)! procedi!\n");

    sprintf(buffer, "ciaooo");

    if (pthread_kill(t2, SIGUSR2) != 0)
    {
        perror("pthread_kill");
    }

    printf("inviato segnale a t2\n");

    printf("heylo\n");
}

void *thread1_entry_point(void *arg)
{
    printf("thread 1 vivo\n");

    if (signal(SIGUSR1, signal1_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    if (pthread_kill(t1, SIGUSR1) != 0)
    {
        perror("pthread_kill");
    }

    while(1) {
        pause();
    }
    printf("muoio! (1)\n");

    return NULL;
}

void signal2_handler(int signum)
{
    printf("ricevuto (2)!\n");

    cookie_counter++;
    printf("cookie counter: %d\n", cookie_counter);

    if (pthread_kill(t1, SIGUSR1) != 0)
    {
        perror("pthread_kill");
    }

    printf("inviato segnale a t1\n");

    printf("ueila\n");
}

void *thread2_entry_point(void *arg)
{
    printf("thread 2 vivo\n");

    if (signal(SIGUSR2, signal2_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    while(1) {
        pause();
    }

    return NULL;
}

int main()
{
    int res;

    res = pthread_create(&t1, NULL, thread1_entry_point, NULL);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    res = pthread_create(&t2, NULL, thread2_entry_point, NULL);
    if (res != 0)
    {
        perror("pthread_create");
        exit(1);
    }

    if(pthread_join(t1, NULL) == -1) {
        perror("pthread_join");
        exit(1);
    }

    printf("bye\n");

    if(pthread_join(t2, NULL) == -1) {
        perror("pthread_join");
        exit(1);
    }

    printf("bye\n");

    return 0;
}
