#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void init_shell_vars() {
    shell_var_count = 0;
}

void update_shell_var(const char *name, const char *value, int i) {
    free(shell_vars[i].value);

    shell_vars[i].value = (char*)malloc(sizeof(char) * (strlen(value) + 1));

    if(shell_vars[i].value == NULL) {
        perror("Allocation failed");
        return;
    }

    strcpy(shell_vars[i].value, value);
}

void create_shell_var(const char *name, const char *value) {
    shell_vars[shell_var_count].name = (char*)malloc(sizeof(char) * (strlen(name) + 1));

    if(shell_vars[shell_var_count].name == NULL) {
        perror("Allocation failed");
        return;
    }

    strcpy(shell_vars[shell_var_count].name, name);

    shell_vars[shell_var_count].value = (char*)malloc(sizeof(char) * (strlen(value) + 1));

    if(shell_vars[shell_var_count].value == NULL) {
        free(shell_vars[shell_var_count].name);
        perror("Allocation failed");
        return;
    }

    strcpy(shell_vars[shell_var_count].value, value);

    shell_var_count++;
}

void set_shell_var(const char *name, const char *value) {
    for (int i = 0; i < shell_var_count; i++) {
        if (strcmp(name, shell_vars[i].name) == 0) {
            update_shell_var(name, value, i);
            return;
        }
    }

    if (shell_var_count < MAX_SHELL_VARS) {
        create_shell_var(name, value);
    } else {
        perror("Maximus Limit Reached");
    }
}

const char* get_shell_var(const char* name) {
    for (int i = 0; i < shell_var_count; i++) {
        if (strcmp(name, shell_vars[i].name) == 0) {
            return shell_vars[i].value;
        }
    }

    return NULL;
}

void print_shell_var() {
    for (int i = 0; i < shell_var_count; i++) {
        printf("%s=%s\n", shell_vars[i].name, shell_vars[i].value);
    }
}

int is_valid_assignment(const char* input) {
    const char *eq = strchr(input, '=');
    return eq != NULL && eq != input;
}

void handle_valid_assignment(const char *input) {
    const char *eq = strchr(input, '=');
    if (!eq) return;

    size_t name_len = eq - input;
    size_t value_len = strlen(eq + 1);

    char* name = malloc(name_len + 1);
    char* value = malloc(value_len + 1);

    if (!name || !value) {
        perror("Allocation failed");
        free(name);
        free(value);
        return;
    }

    strncpy(name, input, name_len);
    name[name_len] = '\0';

    strcpy(value, eq + 1);
    value[value_len] = '\0';

    set_shell_var(name, value);

    free(name);
    free(value);
}

char* expand_variables(const char* input) {
    size_t input_len = strlen(input);
    size_t result_cap = input_len + 1;
    size_t result_len = 0;
    char *result = malloc(result_cap);

    if (!result) return NULL;

    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '$' && (isalpha(input[i + 1]) || input[i + 1] == '_')) {
            i++;

            char varname[256];
            size_t vi = 0;

            while(isalnum(input[i]) || input[i] == '_') {
                varname[vi++] = input[i++];
            }

            varname[vi] = '\0';
            i--;

            const char *val = get_shell_var(varname);
            if (val) {
                size_t val_len = strlen(val);

                if (result_len + val_len + 1 > result_cap) {
                    result_cap = result_len + val_len + 16;
                    result = realloc(result, result_cap);
                    if (!result) return NULL;
                }

                strcpy(&result[result_len], val);
                result_len += val_len;
            }
        } else {
            if (result_len + 2 > result_cap) {
                result_cap = result_len + 16;
                result = realloc(result, result_cap);
                if (!result) return NULL;
            }

            result[result_len++] = input[i];
        }
    }

    result[result_len] = '\0';
    return result;
}