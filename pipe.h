#ifndef PIPE_H
#define PIPE_H

#define MAX_PIPES 16
#define MAX_CMDS  17

#ifndef MAX_ARGS
#define MAX_ARGS 64
#endif

int has_pipe(char **args);
int execute_pipeline(char **args);

#endif