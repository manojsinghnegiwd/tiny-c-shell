#include "pipe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int has_pipe(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            return i;
        }
    }
    return -1;
}

int execute_pipeline(char **args) {
    int index = 0;
    while (args[index] != NULL) {
        index++;
    }
    /* detect pipe in the input and save index of pipes */
    int pipe_flag[MAX_PIPES];
    int count_index = 0, n_pipes = 0;

    pipe_flag[count_index++] = -1;
    for (int i = 0; i < index; i++) {
        if (strcmp(args[i], "|") == 0) {
            if (count_index == MAX_PIPES - 1) {
                fprintf(stderr, "too much pipes");
                return -1;
            }
            pipe_flag[count_index++] = i;
            n_pipes++;
        }
    }
    pipe_flag[count_index++] = index;

    /* devide pipe into commands */
    char *cmds[MAX_CMDS][MAX_ARGS];
    int n_cmds = n_pipes + 1;
    for (int i = 0; i < count_index - 1; i++) {
        for (int j = pipe_flag[i] + 1; j < pipe_flag[i + 1]; j++) {
            cmds[i][j - pipe_flag[i] - 1] = args[j];
        }
        int len = pipe_flag[i + 1] - pipe_flag[i] - 1;
        cmds[i][len] = NULL;
    }

    /* run whole pipeline */
    /* create n-1 pipes */
    int pipes[n_cmds - 1][2];
    for (int i = 0; i < n_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            return -1;
        }
    }

    for (int i = 0; i < n_cmds; i++) {
        pid_t child = fork();
        if (child == -1) {
            perror("fork error");
            return -1;
        }
        if (child == 0) { // child process
            /* redirect stdin -> read end if it's not the first child */
            if (i > 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
            }
            /* redirect stdout -> write end if it's not the last child */
            if (i < n_cmds - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
            }
            
            // close read-end and write-end after redirecting
            for (int j = 0; j < n_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(cmds[i][0], cmds[i]);
            perror(cmds[i][0]);
            exit(EXIT_FAILURE);
        }
    }

    /* close all heads in parent process */
    for (int i = 0; i < n_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* wait all child processes to avoid zombie process */
    for (int i = 0; i < n_cmds; i++) {
        wait(NULL);
    }

    return 0;
}