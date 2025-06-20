#include "history.h"
#include "rawmode.h"
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

void read_input(char* buffer) {
    int len = 0;
    buffer[0] = '\0';

    reset_history_index();
    enable_raw_mode();

    char c;
    while(read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\n') {
            buffer[len] = '\0';
            printf("\n");
            break;
        } else if (c == 127 || c == '\b') {
            if (len > 0) {
                len--;
                buffer[len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        } else if (c == 27) {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
                read(STDIN_FILENO, &seq[1], 1) == 1) {
                    if (seq[0] == '[') {
                        if (seq[1] == 'A') {
                            const char* prevCmd = get_history_up();
                            if (prevCmd) {
                                for (int i = 0; i < len; i++) printf("\b \b");
                                len = snprintf(buffer, MAX_INPUT, "%s", prevCmd);
                                printf("%s", buffer);
                            }
                        } else if (seq[1] == 'B') {
                            const char* nextCmd = get_history_down();
                            for (int i = 0; i < len; i++) printf("\b \b");
                            if (nextCmd) {
                                len = snprintf(buffer, MAX_INPUT, "%s", nextCmd);
                                printf("%s", buffer);
                            } else {
                                len = 0;
                                buffer[0] = '\0';
                            }
                        }
                    }
            }
        } else {
            if (len < MAX_INPUT - 1) {
                buffer[len++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    disable_raw_mode();
}

int main() {
    shell_loop();
    return 0;
}

void shell_loop () {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    init_history();

    while (1) {
        printf("tinyshell> ");
        fflush(stdout);

        read_input(input);

        if (strlen(input) == 0) continue;

        add_to_history(input);

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

        if (strcmp(args[0], "print_history") == 0) {
            print_history();
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

