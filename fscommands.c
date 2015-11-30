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

// Map of command names to the function to invoke
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
  { "formatd", formatd },
  { "chmod", chmod },
  { "su", su }
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

  // Print out the full path
  dump_path(curr_dir_node); 
  printf("\n");
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
  if (argc != 2) {
    fprintf(stderr, "maked(): I take exactly 1 argument\n");
    return;
  }

  // Check permissions
  if (curr_dir_node->entry->perms != RDWR) {
    // Don't have permission
    fprintf(stderr, "maked(): you (user %u) don't have permission\n", curr_user);
    return;
  }
  
  // Allocate new directory entry; put it under current directory
  // FIXME: implement permissions!?
  dir = new_dir(argv[1], curr_user, RDWR, curr_dir_node);

  if (dir == NULL) {
    fprintf(stderr, "maked(): could not make directory\n");
    return;
  }
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
  
  // Check permissions
  if (curr_dir_node->entry->perms != RDWR) {
    // Don't have permission
    fprintf(stderr, "createf(): you (user %u) don't have permission\n", curr_user);
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

  if (file == NULL) {
    fprintf(stderr, "createf(): could not create file\n");
    return;
  }

  // TODO/FIXME: implement users/permissions!!
}

void extendf(size_t argc, char** argv) {
  // TODO/FIXME: implement users/permissions!!
  fs_node* file; // File to extend
  char name[BUFSIZ]; // For saving argument since find() may mangle it
  unsigned int bytes_to_extend; // Size in bytes to shrink the file
  char* end; // For use with strtol()
  unsigned int last_offset; // Offset into the last block
  unsigned int new_size_blocks; // New number of blocks used
  unsigned int new_size_bytes;// New number of blocks used
  int block_index; // Temp index or newly requested blocks
  index_node* inode; // Temp node for traversing
  
  // Check args
  if (argc != 3) {
    fprintf(stderr, "extendf(): Usage: file bytes_to_extend\n");
    return;
  }

  // Get size to extend
  bytes_to_extend = (unsigned int)strtol(argv[2], &end, 10);
  if (*end != '\0') {
    fprintf(stderr, "extendf(): bytes_to_shrink is not an integer\n");
    return;
  }

  // Save the name
  strcpy(name, argv[1]);

  // Find the file
  file = find(argv[1]);
  if (file == NULL) {
    fprintf(stderr, "extendf(): file %s not found\n", name);
    return;
  }

  // Check permissions
  if (file->entry->perms != RDWR) {
    // Don't have permission
    fprintf(stderr, "extendf(): you (user %u) don't have permission\n", curr_user);
    return;
  }

  // Check that it's a regular file
  if (file->entry->type == DIRY) {
    fprintf(stderr, "extendf(): %s is not a regular file\n", name);
    return;
  }

  // Calculate new size
  new_size_bytes = file->entry->size_bytes + bytes_to_extend;

  // Calculate the number of blocks to add, if necessary
  new_size_blocks = new_size_bytes/BLOCK_SIZE;
  if (new_size_bytes % BLOCK_SIZE != 0) {
    new_size_blocks++;
  }

  // Check if there's space
  if (new_size_blocks - file->entry->size_blocks > num_free_blocks) {
    fprintf(stderr, "extendf(): no space left!\n");
    return;
  }

  // Calculate offset into the last block
  last_offset = new_size_bytes < BLOCK_SIZE ?
           new_size_bytes
         : new_size_bytes % BLOCK_SIZE;

  // Request blocks as needed
  for (unsigned int i = 0; i < new_size_blocks - file->entry->size_blocks; i++) {
    block_index = get_block(); 

    if (block_index == -1) {
      fprintf(stderr, "extendf(): impossible happened: no free blocks\n");
      return;
    }

    // Allocate a new inode and insert to tail of the entry's list
    insert_inode(block_index, 0, file->entry);
  }
  
  // Update offsets; every inode's offset except the last will be 0
  // Also overwrite the contents appropriately
  // XXX: this does a lot of unnecessary work
  inode = file->entry->inode_head;
  while (inode != file->entry->inode_tail) {
    inode->offset = 0;
    memset(disk + inode->index * sizeof(block), 0xAA, sizeof(block));

    inode = inode->next;
  }
  
  file->entry->inode_tail->offset = last_offset;
  if (last_offset != 0) {
    memset(disk + inode->index * sizeof(block), 0xAA, last_offset);
  } else {
    memset(disk + inode->index * sizeof(block), 0xAA, sizeof(block));
  }
                  
  // Update size fields
  file->entry->size_bytes = new_size_bytes;
  file->entry->size_blocks = new_size_blocks;
}

void trncf(size_t argc, char** argv) {
  // TODO/FIXME: implement users/permissions!!
  fs_node* file; // File to truncate
  char name[BUFSIZ]; // For saving argument since find() may mangle it
  unsigned int bytes_to_shrink; // Size in bytes to shrink the file
  char* end; // For use with strtol()
  unsigned int last_offset; // Offset into the last block
  unsigned int new_size_blocks; // New number of blocks used
  unsigned int new_size_bytes;// New number of blocks used
  index_node* inode; // For iterating through index nodes to free

  // Check args
  if (argc != 3) {
    fprintf(stderr, "trncf(): Usage: file bytes_to_shrink\n");
    return;
  }

  // Get size to shrink
  bytes_to_shrink = (unsigned int)strtol(argv[2], &end, 10);
  if (*end != '\0') {
    fprintf(stderr, "trncf(): bytes_to_shrink is not an integer\n");
    return;
  }

  // Save the name
  strcpy(name, argv[1]);

  // Find the file
  file = find(argv[1]);
  if (file == NULL) {
    fprintf(stderr, "trncf(): file %s not found\n", name);
    return;
  }

  // Check permissions
  if (file->entry->perms != RDWR) {
    // Don't have permission
    fprintf(stderr, "trncf(): you (user %u) don't have permission\n", curr_user);
    return;
  }

  // Check that it's a regular file
  if (file->entry->type == DIRY) {
    fprintf(stderr, "trncf(): %s is not a regular file\n", name);
    return;
  }

  // Check that we wouldn't be truncating to zero size
  if (file->entry->size_bytes <= bytes_to_shrink) {
    fprintf(stderr, 
      "trncf(): cannot shrink to 0 size: choose a number less than %u\n",
      file->entry->size_bytes);
    return;
  }

  // Calculate new size
  new_size_bytes = file->entry->size_bytes - bytes_to_shrink;

  // Calculate the number of blocks to free
  new_size_blocks = new_size_bytes/BLOCK_SIZE;
  if (new_size_bytes % BLOCK_SIZE != 0) {
    new_size_blocks++;
  }

  // Calculate offset into the last block
  last_offset = new_size_bytes < BLOCK_SIZE ?
           new_size_bytes
         : new_size_bytes % BLOCK_SIZE;

  // Free the necessary blocks
  for (unsigned int i = 0; i < file->entry->size_blocks - new_size_blocks; i++)
  {
    inode = remove_inode_tail(file->entry); 

    if (inode == NULL) {
      fprintf(stderr, "trncf(): impossible happened: file had no blocks\n");
      return;
    }

    // Free the 'disk' block
    free_block(inode->index);

    // Deallocate
    free(inode);
  }

  // 'Clear' unused portion of last block
  if (last_offset != 0) {
    memset(disk + file->entry->inode_tail->index * sizeof(block) 
      + last_offset, 0, sizeof(block) - last_offset);
  }
  
  // Update size fields, offset of last node
  file->entry->size_bytes = new_size_bytes;
  file->entry->size_blocks = new_size_blocks;
  file->entry->inode_tail->offset = last_offset;
}

void deletefd(size_t argc, char** argv) {
  fs_node* file; // File or directory to delete
  char name[BUFSIZ]; // For saving arguments since find() may mangle it

  // TODO/FIXME: implement users/permissions!!
  if (argc < 2) {
    fprintf(stderr, "deletefd(): I take at least 1 argument\n");
  }

  // Delete one-by-one
  for (size_t i = 1; i < argc; i++) {
    // Save the name
    strcpy(name, argv[i]);

    // Find the file/directory
    file = find(argv[i]); 

    if (file == NULL) {
      fprintf(stderr, "deletefd(): file or directory %s not found\n", name);
      continue;
    }

    // Check permissions
    if (file->entry->perms != RDWR) {
      // Don't have permission
      fprintf(stderr, "deletefd(): you (user %u) don't have permission\n", curr_user);
      return;
    }

    // Check if it's a directory
    if (file->entry->type == DIRY) {
      // This is a directory; do recursive delete
      delete_dir(file);

      // For simplicity, navigate back to the root directory in case we just
      // deleted a directory we were under
      curr_dir_node = root_node;

      printf("Moved back to root directory\n");
    } else {
      // This is a regular file
      delete_file(file);
    }
  }
}

void listd(size_t argc, char** argv) {
  fs_node* dir; // Dir to list; temp
  char name[BUFSIZ]; // For saving arguments since find() may mangle it

  if (argc == 1) {
    // No args given; list current directory
    dir = curr_dir_node;
    for (unsigned int c = 0; c < dir->num_children; c++) {
      printf("%s\n", dir->children[c]->entry->name);
    }

    return;
  }

  // List each directory's contents
  for (size_t i = 1; i < argc; i++) {
    // Save the name
    strcpy(name, argv[i]);

    // Find the directory
    dir = find(argv[i]);

    if (dir == NULL) {
      fprintf(stderr, "listf(): %s not found\n", name);
      continue;
    }

    // Check that it's a directory
    if (dir->entry->type != DIRY) {
      fprintf(stderr, "listf(): %s is not a directory\n", name);
      continue;
    }

    // Go through each of the directory's contents and print their names
    printf("%s:\n", name);
    for (unsigned int c = 0; c < dir->num_children; c++) {
      printf("\t%s\n", dir->children[c]->entry->name);
    }
  }
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
      printf("Index = %u, Offset = %u, Address (on disk) = %lu\n", inode->index,
        inode->offset, inode->index * sizeof(block));
      inode = inode->next;
    }
  }
}

void sizef(size_t argc, char** argv) {
  fs_node* file; // File node
  char name[BUFSIZ]; // For saving arguments since find() may mangle it

  // Check args
  if (argc != 2) {
    fprintf(stderr, "sizef(): I take exactly 1 argument\n");
    return;
  }

  // Save name
  strcpy(name, argv[1]); 

  // Find the file
  file = find(argv[1]);

  if (file == NULL) {
    fprintf(stderr, "sizef(): file %s not found\n", name);
    return;
  }

  // Check that it's a regular file
  if (file->entry->type == DIRY) {
    fprintf(stderr, "sizef(): %s is not a regular file\n", name);
    return;
  }

  // Print out its size in blocks and bytes
  printf("Blocks: %u\n", file->entry->size_blocks);
  printf("Bytes: %u\n", file->entry->size_bytes);
}

void movf(size_t argc, char** argv) {
  fs_node* file; // File or directory to move
  fs_node* dir; // Destination directory

  // Check args
  if (argc != 3) {
    fprintf(stderr, "movf(): I take exactly 2 arguments\n");
    return;
  }

  // Find the files/directory
  file = find(argv[1]); 
  if (file == NULL) {
    fprintf(stderr, "movf(): file or directory %s not found\n", argv[1]);
    return;
  }

  // Check permissions
  if (file->entry->perms != RDWR) {
    // Don't have permission
    fprintf(stderr, "movf(): you (user %u) don't have permission\n", curr_user);
    return;
  }

  dir = find(argv[2]); 
  if (dir == NULL) {
    fprintf(stderr, "movf(): directory %s not found\n", argv[2]);
    return;
  }

  // Check permissions
  if (dir->entry->perms != RDWR) {
    // Don't have permission
    fprintf(stderr, "movf(): you (user %u) don't have permission\n", curr_user);
    return;
  }

  // Check that the 2nd argument is a directory
  if (dir->entry->type != DIRY) {
    fprintf(stderr, "movf(): %s is not a directory\n", argv[2]);
    return;
  }

  // Check that the two arguments are distinct
  if (file == dir) {
    fprintf(stderr, "movf(): cannot move directory to itself\n");
    return;
  }

  // Remove file/directory from its current directory
  unlink_child(file);
  
  // Put it in the new directory
  file->parent = dir;
  if (insert_child(file, dir) != 0) {
    fprintf(stderr, "movf(): couldn't add to directory!\n");
    return;
  }
}

void listfb(size_t argc, char** argv) {
  (void) argv;

  block* blocks = (block*)disk; // Pointer to disk as blocks
  int block_index = next_free_block; // Temp for iterating through free blocks

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
  (void) argv;

  // Check args
  if (argc > 1) {
    fprintf(stderr, "dumpfs(): I take no arguments\n");
    return;
  }

  // Dump disk block-by-block
  for (unsigned int i = 0; i < NUM_BLOCKS; i++) {
    dump_block(i);
  }
}

void formatd(size_t argc, char** argv) {
  (void) argv;

  // Check args
  if (argc > 1) {
    fprintf(stderr, "formatd(): I take no arguments\n");
    return;
  }

  printf("Deleting filesystem tree...\n");
  delete_fs_node(&root_node);

  printf("Making new filesystem tree...\n");
  root_node = new_fs_tree();

  printf("Formatting disk...\n");
  init_disk();
}

void chmod(size_t argc, char** argv) {
  char name[BUFSIZ]; // For saving argument since find() may mangle it
  fs_node* file; // File or directory to change permissions on
  permissions new_perms; // New permissions to set

  // Check args
  if (argc != 3) {
    fprintf(stderr, "chmod(): Usage: chmod file [r|w]\n");
    return;
  }

  switch(argv[2][0]) {
    case 'r':
      new_perms = READ;
      break;
    case 'w':
      new_perms = RDWR;
      break;
    default:
      fprintf(stderr, "chmod(): new permissions must be [r]ead-only or read-[w]rite\n"); 
      return;
  }

  // Save name
  strcpy(name, argv[1]);
   
  // Find the file or directory
  file = find(argv[1]); 
  if (file == NULL) {
    fprintf(stderr, "chmod(): file or directory %s not found\n", name);
  }

  // Change the permissions
  file->entry->perms = new_perms;
}

void su(size_t argc, char** argv) {
  user_id new_id; // User id to switch to
  char* end; // For use with strtol()

  // Check args
  if (argc > 2) {
    fprintf(stderr, "su(): I take 1 or 0 arguments\n");
    return;
  }

  if (argc == 1) {
    // No args given; switch to root
    curr_user = ROOT_USER_ID;
    printf("Changed to user: root\n");
    return;
  }

  // Parse user id
  new_id = (user_id)strtol(argv[1], &end, 10);
  if (*end != '\0') {
    fprintf(stderr, "su(): user id must be a integer\n");
    return;
  }

  // Switch to the new user
  curr_user = new_id;
  printf("Changed to user: %u\n", curr_user);
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

