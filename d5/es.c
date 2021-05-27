#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int fd;
char *fsemaphore_name = "/fil_mio_semaforo";
sem_t *fsemaphore;

char *osemaphore_name = "/oil_mio_semaforo";
sem_t *osemaphore;

int segment_size;

int *found_offsets;
int *found_counter;

int integer_division(x, y)
{
    return (x + y - 1) / y;
}

void search_in_file(int offset, char *S)
{
    printf("Searching: %d\n", offset);

    if (sem_wait(fsemaphore) != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    int res = lseek(fd, offset, SEEK_SET);
    if (res == offset - 1)
    {
        perror("lseek");
        exit(1);
    }

    char buffer[segment_size];
    res = read(fd, buffer, segment_size);
    if (res <= 0)
    {
        perror("read");
        exit(1);
    }

    if (sem_post(fsemaphore) != 0)
    {
        perror("sem_post");
        exit(1);
    }

    int *foffsets = calloc(segment_size / strlen(S), sizeof(long));
    int foffsets_count = 0;

    char *found;
    char *current_str = buffer;
    while (current_str - buffer < segment_size && (found = strstr(current_str, S)) != NULL)
    {
        int foffset = found - buffer + offset;

        printf("Trovata! %d\n", foffset);
        current_str += (found - current_str) + strlen(S);

        foffsets[foffsets_count++] = foffset;
    }

    if (sem_wait(osemaphore) != 0)
    {
        perror("sem_wait");
        exit(1);
    }

    if(memcpy(found_offsets + found_counter[0], foffsets, foffsets_count) == NULL) {
        perror("memcpy");
        exit(1);
    }
    found_counter[0] += foffsets_count;

    if (sem_post(osemaphore) != 0)
    {
        perror("sem_post");
        exit(1);
    }
}

void init_mmap(int file_size, char *S)
{
    int fd = open("mmap.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    if(ftruncate(fd, file_size / strlen(S) * sizeof(int)) != 0) {
        perror("ftruncate");
        exit(1);
    }

    found_offsets = mmap(NULL, file_size / strlen(S) * sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         fd, 0);

    if (found_offsets == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    close(fd);

    fd = open("mmap2.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    if(ftruncate(fd, sizeof(int)) != 0) {
        perror("ftruncate");
        exit(1);
    }

    found_counter = mmap(NULL, sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         fd, 0);

    if (found_counter == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    close(fd);
}

void init_semaphores() {
    sem_unlink(fsemaphore_name);
    fsemaphore = sem_open(fsemaphore_name, O_CREAT, 0600, 1);
    if (fsemaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    sem_unlink(osemaphore_name);
    osemaphore = sem_open(osemaphore_name, O_CREAT, 0600, 1);
    if (osemaphore == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    char *filename = argv[1];
    int M = atoi(argv[2]);
    char *S = argv[3];

    init_semaphores();

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    int file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1)
    {
        perror("lseek");
        exit(1);
    }

    init_mmap(file_size, S);

    int K = strlen(S) - 1;
    segment_size = integer_division(file_size, M) + K;

    for (int i = 0; i < M; i++)
    {
        int p = fork();
        if (p == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (p == 0)
        {
            //figlio
            search_in_file(integer_division(file_size, M) * i, S);
            exit(0);
        }
        else
        {
            continue;
        }
    }

    while (wait(NULL) != -1)
        ;

    printf("RECAP:\n");
    for (int i = 0; i < found_counter[0]; i++)
    {
        printf("%d: %d\n", i, found_offsets[i]);
    }

    return 0;
}
