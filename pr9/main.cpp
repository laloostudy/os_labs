#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    char *filename = (char *) "file";

    int rv = chmod(filename, S_IWRITE);
    if (rv == -1)
        perror("chmod");
    else
        printf("chmod() success\n");
    return 0;
}
