#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct
{
    int fd;
    int id;
} thread_parameter_t;

void *thread_entry_point(void *arg)
{
    thread_parameter_t *param = arg;
    printf("[thread %d] ok!\n", param->id);

    char buffer[100];
    for (int i = 0; i < 100; i++)
    {
        int res = sprintf(buffer, "Linea %d del thread %d\n", i, param->id);
        if (res <= 0)
        {
            perror("sprintf");
            exit(1);
        }
        if (write(param->fd, buffer, res) <= 0)
        {
            perror("write");
            exit(1);
        }
    }

    printf("[thread %d] bye!\n", param->id);

    return NULL;
}

int main()
{
    pthread_t threads[10];
    thread_parameter_t params[10];

    char *filename = "file.txt";
    int fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    for (int i = 0; i < 10; i++)
    {
        params[i].id = i;
        params[i].fd = fd;

        int res = pthread_create(&threads[i],
                                 NULL,
                                 thread_entry_point,
                                 &params[i]);

        if (res != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        int res = pthread_join(threads[i], NULL);
        if (res != 0)
        {
            perror("pthread_join");
            exit(1);
        }
    }

    printf("ciao");
    return 0;
}
