#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <ev.h>

void close_data();

char flag;
int fd;
ev_io enter_watcher;
ev_timer timeout_watcher1, timeout_watcher2;


void sig_handler(int fd) {
    printf("Reader fifo has been disabled\n");
}

static void enter_cb(EV_P_ ev_io *w, int revents) {
    printf("Key pressed\n");
    ev_io_stop(EV_A_ w);
    ev_break(EV_A_ EVBREAK_ALL);
}

static void open_cb(EV_P_ ev_timer *w, int revents) {
    errno = 0;
    fd = open("/tmp/my_fifo", O_CREAT | O_WRONLY | O_NONBLOCK);

    if (fd == -1) {
        perror("Error opening fifo");
        sleep(1);
        ev_timer_stop(EV_A_ &timeout_watcher1);
    } else
        ev_timer_start(loop, &timeout_watcher1);
}


static void write_cb(EV_P_ ev_timer *w, int revents) {
    long buf = gethostid();
    write(fd, &buf, sizeof(long));
    printf("Writer: %ld\n", buf);
    sleep(1);
}


void close_data() {
    close(fd);
    unlink("/tmp/my_fifo");
    printf("Data closed\n");
}


int main() {
    struct ev_loop *loop = EV_DEFAULT;
    signal(SIGPIPE, sig_handler);
    errno = 0;

    mkfifo("/tmp/my_fifo", 0644);

    ev_io_init(&enter_watcher, enter_cb, STDIN_FILENO, EV_READ);
    ev_timer_init(&timeout_watcher2, open_cb, 1., 1.);
    ev_timer_init(&timeout_watcher1, write_cb, 1., 1.);

    ev_io_start(loop, &enter_watcher);
    ev_timer_start(loop, &timeout_watcher2);

    ev_run(EV_DEFAULT_ 0);

    close_data();

    printf("The program is completed.\n");
    return 0;
}