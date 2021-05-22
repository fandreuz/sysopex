#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char *src = argv[1];
    char *dest = argv[2];

    // APRI SORGENTE --------------------------------
    int fd = open(src, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        perror("fstat");
        exit(1);
    }

    printf("file size = %lld\n", sb.st_size);

    char *address;

    address = mmap(NULL, sb.st_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   fd, 0);

    if (address == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    if (close(fd) == -1)
    {
        perror("close");
        exit(1);
    }

    // APRI DESTINAZIONE -----------------------------------
    int fd2 = open(dest, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd2 == -1)
    {
        perror("open");
        exit(1);
    }
    if(ftruncate(fd2, sb.st_size) == -1) {
        perror("ftruncate");
        exit(1);
    }

    char *address2;

    address2 = mmap(NULL, sb.st_size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED,
                   fd2, 0);

    if (address2 == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    if (close(fd2) == -1)
    {
        perror("close");
        exit(1);
    }

    // WRITE ----------------------
    memcpy(address2, address, sb.st_size);

    return 0;
}
