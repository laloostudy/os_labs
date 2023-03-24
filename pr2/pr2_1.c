#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>


typedef struct {
    int flag;
    char sym;
} targs;

int stop_flag = 1;
sem_t semaphore;

void *proc1(void *arg) {
    printf("Thread 1 has started working\n");
    targs *args = (targs *) arg;
    while (args->flag != stop_flag) {
        int ret_val = sem_wait(&semaphore);
        if (ret_val == 0){
            printf("function sem_wait OK\n");
        }else{
            perror("sem_wait");
        }

        for (int i = 0; i < 10; i++){
            printf("%c\n", args->sym);
            fflush(stdout);
            sleep(1);
        }
        ret_val = sem_post(&semaphore);
        if (ret_val == 0){
            printf("function sem_post OK\n");
        }else{
            perror("sem_post");
        }
        sleep(1);
    }
    printf("Thread 1 has finished it's work\n");
    pthread_exit((void*)1);
}

void *proc2(void *arg) {
    printf("Thread 2 has started working\n");
    targs *args = (targs *) arg;
    while (args->flag != stop_flag) {
        int ret_val = sem_wait(&semaphore);
        if (ret_val == 0){
            printf("function sem_wait OK\n");
        }else{
            perror("sem_wait");
        }
        for (int i = 0; i < 10; i++){
            printf("%c\n", args->sym);
            fflush(stdout);
            sleep(1);
        }
        ret_val = sem_post(&semaphore);
        if (ret_val == 0){
            printf("function sem_post OK\n");
        }else{
            perror("sem_post");
        }
        sleep(1);
    }
    printf("Thread 2 has finished it's work\n");
    pthread_exit((void*)2);
}


int main() {
    printf("The program has started working\n");
    pthread_t id1, id2;
    int ret_val = sem_init(&semaphore, 0, 1);
    if (ret_val == 0){
        printf("function sem_init OK\n");
    }else{
        perror("sem_init");
    }
    int *exitcode;
    targs arg1, arg2;
    arg1.flag = 0;
    arg1.sym = '1';
    arg2.flag = 0;
    arg2.sym = '2';

    ret_val = pthread_create(&id1, NULL, proc1, &arg1);
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

    arg1.flag = stop_flag;
    arg2.flag = stop_flag;

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

    ret_val = sem_destroy(&semaphore);
    if (ret_val == 0){
        printf("function sem_destroy OK\n");
    }else{
        perror("sem_destroy");
    }
    printf("The program has finished it's work\n");

    return 0;
}
