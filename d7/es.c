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

extern char **environ;

int main()
{
    int fd = open("file.txt", O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    if (ftruncate(fd, 100) != 0)
    {
        perror("ftruncate");
        exit(1);
    }

    char *address = mmap(NULL, 100,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         fd, 0);

    switch (fork())
    {
    case 0:
        //figlio

        if (dup2(fd, 1) == -1)
        {
            perror("dup2");
            exit(1);
        }

        char *new_arguments[] = {"wc /usr/share/dict/words", "-l", "/usr/share/dict/words", NULL};
        char *current_working_directory = getcwd(NULL, 0);

        execve("/usr/bin/wc", new_arguments, environ);
        perror("execve");

        break;
    case -1:
        perror("fork");
        exit(1);
        break;
    default:
        //padre
        if (wait(NULL) == -1)
        {
            perror("wait");
            exit(1);
        }

        printf("%s", address);

        char number[10];
        int i;
        int found_a_digit = 0;
        for (i = 0; i < 100;)
        {
            if (address[i] == ' ')
            {
                if (found_a_digit) break;
                else i++;
            } else {
                found_a_digit = 1;
                i++;
            }
        }

        strncpy(number, address, i+1);
        number[i + 1] = 0;

        printf("Parole: %d", atoi(number));

        break;
    }
}
