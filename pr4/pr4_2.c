#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <poll.h>

const char *semaphore_name = "/name";

int keypressed() {
    struct pollfd fds;
    fds.fd = STDIN_FILENO;
    fds.events = POLLIN;
    fds.revents = 0;
    struct timespec t = {0, 1};
    int retval = ppoll(&fds, 1, &t, NULL);
    return retval;
}

int main() {
    sem_t *semaphore = sem_open(semaphore_name, O_CREAT, 0644, 1);
    FILE *file = fopen("file.txt", "a");
    while (1) {
        sem_wait(semaphore);
        for (int i = 0; i < 5; i++) {
            fputc('2', file);
            fflush(file);
            printf("2");
            fflush(stdout);
            sleep(1);
        }
        sem_post(semaphore);
        sleep(1);
        if (keypressed())
            break;
    }
    fclose(file);
    sem_close(semaphore);
    sem_unlink(semaphore_name);

    return 0;
}

