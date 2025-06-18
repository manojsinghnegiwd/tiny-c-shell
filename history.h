#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 100
#define COMMAND_LENGTH 1024

void init_history();
void add_to_history(const char *cmd);
void print_history();
const char* get_history_up();
const char* get_history_down();
void reset_history_index();

#endif