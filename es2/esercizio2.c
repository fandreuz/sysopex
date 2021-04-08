#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    printf("henlo");
    int occurrences[123] = {0};

    char c;
    int read_result;
    while ((read_result = read(0, &c, 1) != 0)) {
        if (read_result == -1) {
            perror("read");
            exit(1);
        } else {
            occurrences[c] += 1;
        }
    }

    printf("\n\n%s -> %d\n", "\\n", occurrences[10]);
    printf("%s -> %d\n", "' '", occurrences[32]);
    for (int i = 97; i <= 122; i++) {
        printf("%c -> %d\n", i, occurrences[i]);
    }
    exit(0);
}
