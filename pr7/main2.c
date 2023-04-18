#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MQ_MAXMSG 5
#define MQ_MSGSIZE sizeof(long) + 1

const void close_data(void *args);


struct th_args {
    char flag;
    mqd_t mq;
};


void *th_func(void *args) {
    char *flag = &((struct th_args *) args)->flag;
    mqd_t mq = ((struct th_args *) args)->mq;
    char buf[MQ_MSGSIZE];
    ssize_t res;

    while ((int) (*flag) != 10) {
        res = mq_receive(mq, buf, MQ_MSGSIZE, 0);

        if (res >= 0) {
            printf("Receive from queue: %s\n", buf);
        } else {
            perror("Error mq_receive");
        }

        sleep(1);
    }
    pthread_exit((void*)1);
}


const void close_data(void *args) {
    mq_close(((struct th_args *) args)->mq);
    mq_unlink("/myqueue");
}


int main() {
    errno = 0;
    struct mq_attr attr = {0, MQ_MAXMSG, MQ_MSGSIZE, 0, {0}};
    struct th_args params;
    pthread_t th;

    params.mq = mq_open("/myqueue", O_CREAT | O_RDONLY | O_NONBLOCK, 0644, &attr);

    if (pthread_create(&th, NULL, th_func, (void *) (&params))) {
        printf("Thread create failed with error: %s\n", strerror(errno));
        close_data((void *) &params);
        return -errno;
    }

    params.flag = getchar();

    void *status;
    int res = pthread_join(th, &status);
    if (res != 0) {
        printf("Thread join failed with error: %s\n", strerror(res));
        close_data((void *) &params);
        return -errno;
    }


    close_data((void *) &params);
    return 0;
}