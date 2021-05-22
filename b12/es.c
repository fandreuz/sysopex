#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

int main(int argc, char **argv)
{
    DIR *d;
    struct dirent *dir;
    struct stat fileStat;
    char fullpath_buffer[PATH_MAX + 1];
    char partialpath_buffer[PATH_MAX + 1];

    if (opendir(argv[1]))
    {
        while ((dir = readdir(d)) != NULL)
        {
            sprintf(partialpath_buffer, "%s/%s", argv[1], dir->d_name);
            if (stat(partialpath_buffer, &fileStat) < 0)
            {
                perror("stat");
                exit(1);
            }

            if ()
            {
                realpath(dir->d_name, fullpath_buffer);
                printf("%s\n", fullpath_buffer);
            }
        }
        closedir(d);
    }

    return (0);
}
