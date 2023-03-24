#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    pid_t process_id = getpid();
    pid_t p_process_id = getppid();

    for (int i = 0; i < argc; i++) {
        puts(argv[i]);
        sleep(1);
    }

    printf("PID 1: %d\n", process_id);
    printf("PPID 1: %d\n", p_process_id);

    exit(15);
}

