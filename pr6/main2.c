#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/shm.h>
#include <string.h>

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
    key_t key = ftok("file.txt", 1);
    int mem_id = shmget(key, 4096, IPC_CREAT | 0666);
    long *ptr = (long *) shmat(mem_id, NULL, 0);
    sem_t *sem_w = sem_open("/sem_w", O_CREAT, 0644, 1);
    sem_t *sem_r = sem_open("/sem_r", O_CREAT, 0644, 0);
    FILE *file = fopen("file.txt", "a");
    long buf;

    while (1) {
        sem_wait(sem_r);
        memcpy(&buf, ptr, sizeof(long));

        printf("Reader: %ld\n", buf);

        sem_post(sem_w);

        if (keypressed())
            break;
    }

    fclose(file);
    sem_close(sem_r);
    sem_close(sem_w);
    sem_unlink("/sem_w");
    sem_unlink("/sem_r");
    shmdt(ptr);
    shmctl(mem_id, IPC_RMID, (shmid_ds *) ptr);

    return 0;
}

