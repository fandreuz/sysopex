#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int line_length = 0;
int n_lines = 0;
int lines_sum = 0;

void show_stats(void);

int main() {
    char buffer[1];

    while (1)
    {
        int result = read(0, buffer, 1);
        if (result == -1) {
            perror("read");
            exit(1);
        } else if(result == 0) {
            // EOF
            return 0;
        }
        else
        {
            if (buffer[0] == '\n') {
                n_lines++;
                lines_sum += line_length;

                show_stats();

                line_length = 0;
            }
            else
            {
                line_length++;
            }
        }
    }
}

void show_stats(void) {
    printf("Read %d lines. Current line: %d character(s). Average: %lf\n", n_lines, line_length, (double)lines_sum / (double)n_lines);
}
