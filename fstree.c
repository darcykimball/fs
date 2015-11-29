#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fstree.h"
#include "filesystem.h"

// Helper for delete_inode_list()
static void delete_inode(index_node* inode);

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
  new_entry->inode_head = new_entry->inode_tail = NULL;

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
  new_node->parent = parent;

  // Check for special case: creating root node
  if (parent == NULL) {
    return new_node;
  }

  // Insert the new child
  insert_child(new_node, parent);

  return new_node;
}

void delete_fs_node(fs_node** nodepp) {
  fs_node* nodeptr = *nodepp; // For readability

  // Delete each of the child nodes
  for (unsigned int i = 0; i < nodeptr->num_children; i++) {
    delete_fs_node(&(nodeptr->children[i]));
  }

  // Delete the index node list
  delete_inode_list(nodeptr->entry);

  // Delete the file entry
  free(nodeptr->entry);

  // Delete this
  free(nodeptr);

  // Invalidate the pointer to node
  *nodepp = NULL;
}

fs_node* new_fs_tree() {
  fs_entry* dir_entry; // The directory entry

  printf("new_fs_tree(): Initializing filesystem tree...\n");

  dir_entry = new_fs_entry(
    "root",
    DIRY,
    ROOT_USER_ID,
    RDWR,
    0
  );

  return new_fs_node(dir_entry, NULL);
}

void insert_inode(unsigned int block_index, unsigned int offset,
  fs_entry* entry) {
  index_node* new_inode; // New inode
  
  // Allocate/initialize new index node
  new_inode = (index_node*)malloc(sizeof(index_node));
  new_inode->index = block_index;
  new_inode->offset = offset;
  new_inode->next = NULL;

  // Check for special case: first insertion, i.e. tail is NULL
  if (entry->inode_tail == NULL) {
    entry->inode_head = entry->inode_tail = new_inode;
  } else {
    entry->inode_tail->next = new_inode;
    entry->inode_tail = new_inode;
  }
}

int insert_child(fs_node* new_child, fs_node* dir) {
  // Sanity check
  if (new_child == NULL || dir == NULL) {
    fprintf(stderr, "insert_child(): null ptr!!\n");
    return -1;
  }

  // Link to parent; check for space first
  if (dir->num_children == MAX_DIR_FILES) {
    // No space
    fprintf(stderr, "insert_child(): no space in parent's children list!");
    return -1;
  }

  dir->children[dir->num_children] = new_child;
  dir->num_children++;

  return 0;
}

void unlink_child(fs_node* child) {
  fs_node* parent = child->parent; // For convenience
  unsigned int index = 0; // Index of child in parent's children

  // Look through each of the children
  for (unsigned int c = 0; c < parent->num_children; c++) {
    if (parent->children[c] == child) {
      // Found it; store the index
      index = c;
      break;
    }
  }

  // Shift over the rest
  for (unsigned int c = index; c < parent->num_children - 1; c++) {
    parent->children[c] = parent->children[c + 1];
  }

  // Update count, parent
  parent->num_children--;
  child->parent = NULL;
}

void delete_inode_list(fs_entry* entry) {
  // Check for empty list
  if (entry->inode_head == NULL) {
    return;
  } 

  // Recursively delete nodes
  delete_inode(entry->inode_head);
}

static void delete_inode(index_node* inode) {
  if (inode->next != NULL) {
    delete_inode(inode->next);
  }

  free(inode); 
}

void dump_path(fs_node* node) {
  // Print the parent before this
  if (node->parent != NULL) {
    dump_path(node->parent);
    printf("/");
  }

  printf("%s", node->entry->name);
}

