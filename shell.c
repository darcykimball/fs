#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "shell.h"

#define MAX_N_TOKENS 100

static char const PROMPT_STR[] = ">>";

static size_t tokenize(char* str, char** tokens, size_t max_tokens);
static command_fn lookup(command_pair* commands, size_t n_commands,
  char const* command);  

void loop(command_pair* command_map, size_t n_commands) {
  char* input = NULL; // Buffer to hold raw line input
  size_t line_len = 0; // Number of characters in read line
  char* tokens[MAX_N_TOKENS]; // To hold tokens for a given line
  size_t n_tokens; // Number of tokens for a given line

  // Main loop
  while (1) {
    // Print prompt
    printf("%s", PROMPT_STR);

    // Get input
    if (getline(&input, &line_len, stdin) == -1) {
      // Error reading or EOF
      break;
    }

    // Tokenize
    n_tokens = tokenize(input, tokens, MAX_N_TOKENS);

    // FIXME: remove!
    // echo back
    printf("%s", input);
  }

  // Free buffer alloc'd by getline()
  free(input);
}

// Split input string on whitespace, setting 'tokens' to contain the array
// of token strings. Returns the number of tokens.
static size_t tokenize(char* str, char** tokens, size_t max_tokens) {
  char* token; // Token returned by strtok() 
  size_t token_count = 0; // Number of tokens consumed so far
  char* strarg = str; // Argument to strtok() must be 'str' on first call,
                      // NULL after

  // Consume as many tokens as possible
  while ((token = strtok(strarg, " ")) != NULL) {
    // Setup argument for subsequent calls
    if (token_count == 0) {
      strarg = NULL;
    }

    // Check for space
    if (token_count < max_tokens) {
      tokens[token_count] = token;
    } else {
      // No space left
      break;
    }
  }

  return token_count;
}

// Look up a command; returns NULL if not present
static command_fn lookup(command_pair* commands, size_t n_commands,
  char const* command) {
  // Look through available commands one-by-one
  for (size_t i = 0; i < n_commands; i++) {
    // Compare input with current command in map
    if (strcmp(commands[i].command_str, command) == 0) {
      // Found it
      return commands[i].fn;
    }
  } 

  // Not found
  return NULL;
}
