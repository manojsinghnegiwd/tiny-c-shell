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
    /* devide pipe into commands */
    char *cmds[MAX_CMDS][MAX_ARGS];
    int i = 0, j = 0;
    for (int k = 0; args[k] != NULL; k++) {
        if (strcmp(args[k], "|") == 0) {
            cmds[i][j] = NULL;
            i++;
            j = 0;
        }
        else {
            cmds[i][j++] = args[k];
        }
    }
    cmds[i][j] = NULL;
    int n_cmds = i + 1;

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