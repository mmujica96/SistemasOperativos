// timepipe.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Proceso hijo
        close(pipefd[0]);  // Cerrar la lectura del pipe

        struct timeval start_time;
        gettimeofday(&start_time, NULL);

        write(pipefd[1], &start_time, sizeof(struct timeval));
        close(pipefd[1]);

        // Preparar el comando para execvp
        char *cmd[argc];
        for (int i = 1; i < argc; i++) {
            cmd[i - 1] = argv[i];
        }
        cmd[argc - 1] = NULL;

        execvp(cmd[0], cmd);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {  // Proceso padre
        close(pipefd[1]);  // Cerrar la escritura del pipe

        struct timeval start_time;
        read(pipefd[0], &start_time, sizeof(struct timeval));
        close(pipefd[0]);

        wait(NULL);

        struct timeval end_time;
        gettimeofday(&end_time, NULL);

        long seconds = end_time.tv_sec - start_time.tv_sec;
        long microseconds = end_time.tv_usec - start_time.tv_usec;
        double elapsed = seconds + microseconds*1e-6;

        printf("Elapsed time: %.6f seconds\n", elapsed);
    }

    return 0;
}
