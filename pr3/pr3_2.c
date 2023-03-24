#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    pid_t process_id = getpid();
    pid_t p_process_id = getppid();

    printf("PID 2: %d\n", process_id);
    printf("PPID 2: %d\n", p_process_id);

    int status = 0;
    pid_t pid = fork();
    if (pid == 0) {
        char *argv[] = {(char *) "arg1", (char *) "arg2", (char *) "arg3", NULL};
        setenv("PATH", ".", 1);
        extern char **environ;
        execvpe("pr3_1", argv, environ);
    } else if (pid > 0) {
        while (waitpid(pid, &status, WNOHANG) == 0) {
            printf("Waiting for child process\n");
            usleep(500000);
        }
        printf("Status code: %d\n", WEXITSTATUS(status));
    }
    return 0;
}