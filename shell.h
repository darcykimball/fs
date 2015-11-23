#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>

#define MAX_CMD_LEN 12

typedef void (*command_fn)(size_t argc, char** argv);

typedef struct {
  char command_str[MAX_CMD_LEN]; // Name of command
  command_fn fn; // Command to be invoked 
} command_pair;

void loop(command_pair* command_map, size_t n_commands); // Main interpreter loop

#endif // SHELL_H
