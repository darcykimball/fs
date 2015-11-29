#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fstree.h"
#include "filesystem.h"

fs_entry* new_fs_entry(char* name, file_type type, user_id user,
  permissions perms, unsigned int size_bytes) {
  size_t nmlen; // Length of name
  fs_entry* new_entry; // Return value

  // Sanity check: name is short enough?
  nmlen = strlen(name);
  if (nmlen > MAX_NM_LEN - 1) {
    // Too long.
    return NULL;
  }
  
  // Allocate
  new_entry = (fs_entry*)malloc(sizeof(fs_entry));

  // Initialize
  strcpy(new_entry->name, name); 
  new_entry->type = type;
  new_entry->user = user;
  new_entry->perms= perms;
  new_entry->inodes = NULL;

  if (type == DIRY) {
    new_entry->size_bytes = 0;
    new_entry->size_blocks = 0;
  } else {
    new_entry->size_bytes = size_bytes;
    new_entry->size_blocks = size_bytes/BLOCK_SIZE;

    // Account for possible partially occupied block
    if (size_bytes % BLOCK_SIZE != 0) {
      new_entry->size_blocks++;
    }
  }

  return new_entry;
}

fs_node* new_fs_node(fs_entry* entry, fs_node* parent) {
  fs_node* new_node; // Return value

  // Allocate/initialize
  new_node = (fs_node*)malloc(sizeof(fs_node));

  new_node->entry = entry;
  memset(new_node->children, 0, sizeof(new_node->children));
  new_node->num_children = 0;

  // Link to parent; check for space first
  if (parent->num_children == MAX_DIR_FILES) {
    // No space
    fprintf(stderr, "new_fs_node(): no space in parent's children list!");
    return NULL;
  }

  // Check for special case: creating root node
  if (parent != NULL) {
    parent->children[parent->num_children] = new_node;
    parent->num_children++;
  }

  return new_node;
}

void delete_fs_node(fs_node** nodepp) {
  fs_node* nodeptr = *nodepp; // For readability

  // Delete each of the child nodes
  for (unsigned int i = 0; i < nodeptr->num_children; i++) {
    delete_fs_node(&(nodeptr->children[i]));
  }

  // Delete the file entry
  free(nodeptr->entry);

  // Delete this
  free(nodeptr);

  // Invalidate the pointer to node
  *nodepp = NULL;
}

fs_node* new_fs_tree() {
  fs_entry* dir_entry; // The directory entry
  permissions perms = { 1, 1 }; // Both read/write

  dir_entry = new_fs_entry(
    "root",
    DIRY,
    ROOT_USER_ID,
    perms,
    0
  );

  return new_fs_node(dir_entry, NULL);
}
