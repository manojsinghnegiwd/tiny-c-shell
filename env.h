#ifndef ENV_H
#define ENV_H

#include <stdlib.h>
#include <string.h>

#define MAX_SHELL_VARS 100

typedef struct {
    char *name;
    char *value;
} Shell_var_t;

static Shell_var_t shell_vars[MAX_SHELL_VARS];
static int shell_var_count = 0;

char* expand_variables(const char *input);
int is_valid_assignment(const char *input);
void handle_valid_assignment(const char *input);
void init_shell_vars();
void set_shell_var(const char *name, const char *value);
const char* get_shell_var(const char *name);
void print_shell_var();


#endif