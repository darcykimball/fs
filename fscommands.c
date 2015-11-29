#include <stddef.h>
#include <stdlib.h>
#include "fscommands.h"
#include "filesystem.h"
#include "fsfile.h"

#define MAX_N_TOKENS 100 // Maximum filepath tokens

// FIXME
fs_node* curr_dir_node = NULL;
fs_node* root_node = NULL;
user_id curr_user = 0; // FIXME!!

// Split input string on slashes, setting 'tokens' to contain the array
// of token strings. Returns the number of tokens.
static size_t splitpath(char* str, char** tokens, size_t max_tokens);

// Helper function; find the file/directory with the given path. Returns NULL
// if not found
static fs_node* find(char* filepath);

command_pair command_map[] = {
  { "currentd", currentd },
  { "chdir", chdir },
  { "maked", maked },
  { "createf", createf },
  { "extendf", extendf },
  { "trncf", trncf },
  { "deletefd", deletefd },
  { "listd", listd },
  { "listf", listf },
  { "sizef", sizef },
  { "movf", movf },
  { "listfb", listfb },
  { "dumpfs", dumpfs },
  { "formatd", formatd }
};

// List current directory
void currentd(size_t argc, char** argv) {
  // Check for valid args
  if (argc > 1) {
    fprintf(stderr, "currentd(): I take no arguments\n");
    return;
  } 

  (void)argv; // Avoid compiler warning...

  // Sanity check
  if (curr_dir_node == NULL || curr_dir_node->entry->type != DIRY) {
    fprintf(stderr, "currentd(): NULL or non-directory curr_dir_node!!\n");
    return;
  }

  // Print out the name
  printf("%s\n", curr_dir_node->entry->name);
}

void chdir(size_t argc, char** argv) {
  // Check args
  if (argc > 2) {
    fprintf(stderr, "chdir(): I take 1 or 0 arguments\n");
    return;
  }

  if (argc == 1) {
    // No args given; change to root directory
    curr_dir_node = root_node;
    return;
  }
    
  // Try to find the filepath
  // TODO
}

void maked(size_t argc, char** argv) {
}

void createf(size_t argc, char** argv) {
  fs_node* file; // Node to be created for new file
  unsigned int size_bytes; // Size in bytes of the new file
  char* end; // For use with strtol()
  file_type type; // File type
  permissions perms = { 1, 1 };

  // Check args
  if (argc != 4) {
    fprintf(stderr, "createf(): Usage: createf filename file_type file_size\n");
    fprintf(stderr, " Usage: file_type can be [t]xt, [e]xe, [i]mg, [d]oc, or [m]ov\n");
    return;
  }
  
  size_bytes = (unsigned int)strtol(argv[3], &end, 10);
  if (*end != '\0') {
    fprintf(stderr, "createf(): file_size is not an integer\n");
    return;
  }

  if (size_bytes == 0) {
    fprintf(stderr, "createf(): file_size must be > 0\n");
    return;
  }

  // Check first char of file_type arg
  switch (argv[2][0]) {
    case 't':
      type = TXT;   
      break;
    case 'e':
      type = EXE;   
      break;
    case 'i':
      type = IMG;   
      break;
    case 'd':
      type = DOC;   
      break;
    case 'm':
      type = MOV;   
      break;
    default:
      type = TXT;
      break;
  }

  printf("Creating new file with name %s, type %d, and size %u\n",
    argv[1], type, size_bytes);
  
  file = new_file(argv[1], type, curr_user, perms, size_bytes, curr_dir_node);

  // TODO/FIXME: implement users/permissions!!
}

void extendf(size_t argc, char** argv) {
}

void trncf(size_t argc, char** argv) {

}

void deletefd(size_t argc, char** argv) {
}

void listd(size_t argc, char** argv) {
}

void listf(size_t argc, char** argv) {
}

void sizef(size_t argc, char** argv) {
}

void movf(size_t argc, char** argv) {
}

void listfb(size_t argc, char** argv) {
  block* blocks = (block*)disk; // Pointer to disk as blocks
  int block_index = next_free_block; // Temp for iterating through free blocks

  printf("%d", next_free_block);

  // Check args
  if (argc > 1) {
    fprintf(stderr, "listfb(): I take no arguments\n");
    return;
  }

  // Go through the free list, printing out the indices of free blocks
  printf("Free list indices:\n");
  while (block_index != -1) {
    printf("%d\n", block_index);
    block_index = blocks[block_index].next;
  }
}

void dumpfs(size_t argc, char** argv) {
  // Dump disk block-by-block
  for (unsigned int i = 0; i < NUM_BLOCKS; i++) {
    dump_block(i);
  }
}

void formatd(size_t argc, char** argv) {
  printf("Deleting filesystem tree...\n");
  delete_fs_node(&root_node);

  printf("Making new filesystem tree...\n");
  root_node = new_fs_tree();

  printf("Formatting disk...\n");
  init_disk();
}

// Split input string on slashes, setting 'tokens' to contain the array
// of token strings. Returns the number of tokens.
static size_t splitpath(char* str, char** tokens, size_t max_tokens) {
  char* token; // Token returned by strtok() 
  size_t token_count = 0; // Number of tokens consumed so far
  char* strarg = str; // Argument to strtok() must be 'str' on first call,
                      // NULL after

  // Consume as many tokens as possible
  while ((token = strtok(strarg, "/")) != NULL) {
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

    token_count++;
  }

  return token_count;
}

static fs_node* find(char* filepath) {
  char* tokens[MAX_N_TOKENS]; // To hold filename/dirname tokens
  size_t n_tokens; // Number of tokens in the filepath
  fs_node* curr_node = root_node; // Temp node for traversing
  int found = 0; // Flag for finding filenames

  // Split the filepath
  n_tokens = splitpath(filepath, &tokens, MAX_N_TOKENS);

  // Start traversing from the top ("/")
  // FIXME/TODO
}

