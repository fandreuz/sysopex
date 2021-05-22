#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>

void read_lines_from_file(char *filename);
void save_line(char *line);
void check_word(char *word);

typedef char *my_string;

my_string *file_content;
int file_content_size = 0;
int file_content_free_cell = 0;

int main()
{
    file_content_size = 1;

    file_content = malloc(file_content_size * sizeof(my_string));
    if (file_content == NULL)
    {
        perror("malloc");
        exit(1);
    }
    file_content[0] = NULL;

    read_lines_from_file("american-english");

    // SVOLGO IL PUNTO 2-FULL

    printf("====== OUTPUT ======\n");

    char word_buffer[256];
    char small_buffer[1];
    int read_result = 0;

    int current_word_idx = 0;

    while ((read_result = read(0, small_buffer, 1)) == 1)
    {
        if (small_buffer[0] == ' ' || small_buffer[0] == '\n')
        {
            word_buffer[current_word_idx] = 0;
            check_word(word_buffer);
            current_word_idx = 0;
        }
        else
        {
            word_buffer[current_word_idx] = small_buffer[0];
            current_word_idx++;
        }
    }
    if (read_result == -1)
    {
        perror("read");
        exit(1);
    }

    return 0;
}

void check_word(char *word)
{
    int fork_res = fork();
    if (fork_res > 0)
    {
        // padre
        if (wait(&fork_res) == -1)
        {
            perror("wait");
            exit(1);
        }
    }
    else if (fork_res == 0)
    {
        // figlio
        for (int i = 0; i < file_content_free_cell; i++)
        {
            if (strcmp(word, file_content[i]) == 0)
            {
                exit(0);
            }
        }
        printf("%s\n", word);
        exit(0);
    }
    else
    {
        perror("fork");
        exit(1);
    }
}

void save_line(char *line)
{
    file_content[file_content_free_cell] = strdup(line);
    file_content_free_cell++;

    if (file_content_free_cell == file_content_size)
    {
        file_content_size *= 2;
        file_content = realloc(file_content, file_content_size * sizeof(my_string));
        if (file_content == NULL)
        {
            perror("realloc");
            exit(1);
        }

        for (int i = file_content_size / 2; i < file_content_size; i++)
        {
            file_content[i] = NULL;
        }
    }
}

void read_lines_from_file(char *filename)
{
    int fd;
    int bytes_read;

    char *buffer;
    int buffer_size = 2;

    buffer = malloc(buffer_size * sizeof(char));
    if (buffer == NULL)
    {
        perror("malloc");
        exit(1);
    }

    int buffer_len;

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    int count_lines = 0;

    while ((bytes_read = read(fd, buffer, buffer_size)) > 0)
    {
        int pos = 0;
        while (buffer[pos] != '\n' && pos < bytes_read)
            pos++;

        if (pos == bytes_read && buffer_size == bytes_read)
        {
            buffer_size = buffer_size * 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL)
            {
                perror("realloc");
                exit(1);
            }

            if (lseek(fd, -bytes_read, SEEK_CUR) == -1)
            {
                perror("lseek");
                exit(1);
            }
        }
        else
        {
            count_lines++;

            buffer[pos] = 0;
            buffer_len = pos;

            save_line(buffer);

            int new_offset = pos + 1 - bytes_read;
            if (new_offset != 0 && lseek(fd, new_offset, SEEK_CUR) == -1)
            {
                perror("lseek");
                exit(1);
            }
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
    }

    free(buffer);
}
