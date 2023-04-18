#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ev.h>


void close_data();

char flag;
int fd;

ev_io enter_watcher, read_watcher;

static void enter_cb(EV_P_ ev_io *w, int revents) {
    printf("\nKey pressed\n");
    ev_io_stop(EV_A_ w);
    ev_break(EV_A_ EVBREAK_ALL);
}

static void read_cb(EV_P_ ev_io *w, int revents) {
    errno = 0;
    long buf;

    buf = 0;
    if (read(fd, &buf, sizeof(long)) > 0) {
        printf("Reader: %ld\n", buf);
    } else {
        printf("Reading fifo is empty\n");
    }
    sleep(1);
}


void close_data() {
    close(fd);
    unlink("/tmp/my_fifo");
    printf("Data closed\n");
}


int main() {
    struct ev_loop *loop = EV_DEFAULT;
    errno = 0;
    pthread_t th_r;

    mkfifo("/tmp/my_fifo", 0644);

    fd = open("/tmp/my_fifo", O_CREAT | O_RDONLY | O_NONBLOCK);

    ev_io_init(&enter_watcher, enter_cb, STDIN_FILENO, EV_READ);
    ev_io_init(&read_watcher, read_cb, fd, EV_READ);

    ev_io_start(loop, &read_watcher);
    ev_io_start(loop, &enter_watcher);

    ev_run(EV_DEFAULT_ 0);

    close_data();
    printf("The program is completed.\n");
    return 0;
}