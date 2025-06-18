#include <sys/syslimits.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_timeval.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

void shell_loop();

int main() {
    shell_loop();
    return 0;
}

void shell_loop () {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf("tinyshell> ");
        fflush(stdout);

        if(fgets(input, MAX_INPUT, stdin) == NULL) {
            perror("fgets");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                perror("cd failed");
            } else if (chdir(args[1]) != 0) {
                perror("cd failed");
            }
            continue;
        }

        if (strcmp(args[0], "pwd") == 0) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("pwd failed");
            }
            continue;
        }

        struct timeval start, end;
        gettimeofday(&start, NULL);

        pid_t pid = fork();

        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            gettimeofday(&end, NULL);

            long seconds = end.tv_sec - start.tv_sec;
            long usec = end.tv_usec - start.tv_usec;
            double elapsed = seconds * 1000.0 + usec / 1000.0;

            printf("Exit status: %d\n", WEXITSTATUS(status));
            printf("Time taken: %.3f ms\n", elapsed);
        } else {
            perror("fork failed");
        }
    }
}

