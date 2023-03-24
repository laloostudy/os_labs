#include <pthread.h>
#include <unistd.h>
#include <stdio.h>


typedef struct {
    int flag;
    char sym;
} targs;


void *proc1(void *arg) {
    printf("Thread 1 has started working\n");
    targs *args = (targs *) arg;
    while (args->flag == 0) {
        printf("%c\n", args->sym);
        fflush(stdout);
        sleep(1);
    }
    printf("Thread 1 has finished it's work\n");
    pthread_exit((void*)1);
}

void *proc2(void *arg) {
    printf("Thread 2 has started working\n");
    targs *args = (targs *) arg;
    while (args->flag == 0) {
        printf("%c\n", args->sym);
        fflush(stdout);
        sleep(1);
    }
    printf("Thread 2 has finished it's work\n");
    pthread_exit((void*)2);
}


int main() {
    printf("The program has started working\n");
    pthread_t id1, id2;
    int *exitcode;
    targs arg1, arg2;
    arg1.flag = 0;
    arg1.sym = '1';
    arg2.flag = 0;
    arg2.sym = '2';

    int ret_val = pthread_create(&id1, NULL, proc1, &arg1);
    if (ret_val == 0) {
        printf("function pthread_create OK\n");
    } else {
        perror("pthread_create");
    }

    ret_val = pthread_create(&id2, NULL, proc2, &arg2);
    if (ret_val == 0) {
        printf("function pthread_create OK\n");
    } else {
        perror("pthread_create");
    }
    printf("The program is waiting for the key to be pressed\n");
    getchar();
    printf("The key is pressed\n");
    arg1.flag = 1;
    arg2.flag = 1;
    ret_val = pthread_join(id1, (void**)&exitcode);
    printf("exitcode = %p\n",exitcode);
    if (ret_val == 0) {
        printf("function pthread_join OK\n");
    } else {
        perror("pthread_join");
    }
    ret_val = pthread_join(id2, (void**)&exitcode);
    printf("exitcode = %p\n",exitcode);
    if (ret_val == 0) {
        printf("function pthread_join OK\n");
    } else {
        perror("pthread_join");
    }
    printf("The program has finished it's work\n");

    return 0;
}
