#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

int machine_state = 1;
int dt = 1000;

void increase_dt()
{
    dt += 100;
    if (dt > 10000)
    {
        dt = 10000;
    }
}

void decrease_dt()
{
    dt -= 100;
    if (dt < 0)
    {
        dt = 1;
    }
}

void toggle_machine_state()
{
    machine_state = (machine_state + 1) % 2;
}

void process_signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        decrease_dt();
    }
    else if (signum == SIGUSR2)
    {
        increase_dt();
    }
    else if (signum == SIGTERM)
    {
        toggle_machine_state();
    }
}

int main()
{
    // registro i segnali
    if (signal(SIGUSR1, process_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    if (signal(SIGUSR2, process_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    if (signal(SIGTERM, process_signal_handler) == SIG_ERR)
    {
        perror("signal");
        exit(1);
    }

    while(1) {
        if (machine_state == 1) {
            printf("COOKIE!\n");
        } else {
            printf("ZzzZ..\n");
        }

        printf("aspetto %f secondi\n", (double)dt / (double)1000);
        sleep((double)dt / (double)1000);
    }
}
