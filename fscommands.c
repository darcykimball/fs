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
static size_t splitpath(char* str, char* tokens[], size_t max_tokens);

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
  fs_node* dir; // Directory to change to

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
  dir = find(argv[1]); 

  if (dir == NULL) {
    fprintf(stderr, "chdir(): directory %s not found\n", argv[1]);
    return;
  }

  // Check if it really is a directory
  if (dir->entry->type != DIRY) {
    fprintf(stderr, "chdir(): %s is not a directory\n", argv[1]);
    return;
  }

  // Change to the directory
  curr_dir_node = dir;
}

void maked(size_t argc, char** argv) {
  fs_node* dir; // Node to be created for new direcory

  // Check args
  if (argc > 2) {
    fprintf(stderr, "maked(): I take exactly 1 argument\n");
    return;
  }
  
  // Allocate new directory entry; put it under current directory
  // FIXME: implement permissions!?
  dir = new_dir(argv[1], curr_user, RDWR, curr_dir_node);
}

void createf(size_t argc, char** argv) {
  fs_node* file; // Node to be created for new file
  unsigned int size_bytes; // Size in bytes of the new file
  char* end; // For use with strtol()
  file_type type; // File type

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
  
  file = new_file(argv[1], type, curr_user, RDWR, size_bytes, curr_dir_node);

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
  fs_node* file; // File node; temp
  index_node* inode; // Index node; for iterating through inode lists

  // Check args
  if (argc < 2) {
    fprintf(stderr, "listf(): I take at least one argument\n");
    return;
  }

  // List each file's index block addresses
  for (size_t i = 1; i < argc; i++) {
    // Try to find the file
    file = find(argv[i]);
    if (file == NULL) {
      fprintf(stderr, "listf(): file %s not found\n", argv[i]);
      continue;
    }

    // Check if it's a directory
    if (file->entry->type == DIRY) {
      fprintf(stderr, "listf(): file %s is a directory\n", argv[i]);
      continue;
    }

    printf("Index nodes for %s:\n", argv[i]);

    // Go through its index block list
    inode = file->entry->inode_head; 
    while (inode != NULL) {
      printf("Index = %u, Offset = %u\n", inode->index, inode->offset);
      inode = inode->next;
    }
  }
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
static size_t splitpath(char* str, char* tokens[], size_t max_tokens) {
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
  fs_node* curr_node; // Temp node for traversing
  int found_index = -1; // Flag for finding filenames
  unsigned int num_children; // Number of children in a node

  // Split the filepath
  n_tokens = splitpath(filepath, tokens, MAX_N_TOKENS);

  // Start traversing from the top ("/"); root if leading slash, current dir
  // otherwise
  if (filepath[0] == '/') {
    // Start at root
    curr_node = root_node;
  } else {
    // Start at current directory
    curr_node = curr_dir_node;
  }

  // Look for matching names, token by token, traversing down when a name
  // matches. If no name is found on a level, the search was unsuccessful
  for (size_t i = 0; i < n_tokens; i++) {
    // Check for this token as a name in this node's children
    found_index = -1;
    num_children = curr_node->num_children;

    // XXX: this is inefficient
    for (unsigned int c = 0; c < num_children; c++) {
      if (strcmp(curr_node->children[c]->entry->name, tokens[i]) == 0) {
        // Found match
        found_index = c;
      }
    }

    // Check that we found a matching name on this level
    if (found_index == -1) {
      // Something didn't match; exit
      return NULL;
    } else {
      // Move on deeper
      curr_node = curr_node->children[found_index];
    }
  }

  return curr_node;
}

