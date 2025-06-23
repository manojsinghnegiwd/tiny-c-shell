#include "history.h"
#include <stdio.h>
#include <string.h>

static char history[MAX_HISTORY][COMMAND_LENGTH];
static int history_count = 0;
static int current_index = 0;

void init_history() {
    history_count = 0;
    current_index = 0;
}

void add_to_history(const char *cmd) {
    if (history_count < MAX_HISTORY) {
        strncpy(history[history_count], cmd, COMMAND_LENGTH - 1);
        history[history_count][COMMAND_LENGTH -1] = '\0';
        history_count++;
    }

    current_index = history_count;
}

const char* get_history_up() {
    if (current_index > 0) {
        current_index--;
        return history[current_index];
    }

    return NULL;
}

const char* get_history_down() {
    if (current_index < history_count - 1) {
        current_index++;
        return history[current_index];
    } else {
        current_index = history_count;
        return "";
    }

    return NULL;
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%s\n", history[i]);
    }
}

void reset_history_index() {
    current_index = history_count;
}