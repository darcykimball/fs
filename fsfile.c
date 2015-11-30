#include <stdio.h>
#include "filesystem.h"
#include "fsfile.h"

fs_node* new_file(char* name, file_type type, user_id user, permissions perms,
  unsigned int size_bytes, fs_node* curr_dir_node) {
  fs_entry* entry; // Entry for new file
  fs_node* node; // Return value
  unsigned int num_blocks_needed; // Number of blocks new file should occupy
  unsigned int last_offset; // Offset into the last block
  int block_index; // Current block index
  block* blocks = (block*) disk; // Pointer to disk as blocks

  // Create a new filesystem entry and node for the file
  entry = new_fs_entry(name, type, user, perms, size_bytes);

  // Sanity check
  if (entry == NULL) {
    fprintf(stderr, "new_file(): couldn't init entry!\n");
    return NULL;
  }

  // Check for duplicate named file in current directory
  for (unsigned int i = 0; i < curr_dir_node->num_children; i++) {
    if (strcmp(name, curr_dir_node->children[i]->entry->name) == 0) {
      fprintf(stderr, "new_file(): there already exists a file by that name\n");
      return NULL;
    }
  }

  // Calculate the number of blocks needed
  num_blocks_needed = size_bytes/BLOCK_SIZE;
  if (size_bytes % BLOCK_SIZE != 0) {
    num_blocks_needed++;
  }

  // Calculate offset into the last block
  last_offset = size_bytes < BLOCK_SIZE ?
           size_bytes
         : size_bytes % BLOCK_SIZE;

  // Check for space
  if (num_blocks_needed > num_free_blocks) {
    fprintf(stderr, "new_file(): no available space\n");
    return NULL;
  }
 
  // Link a new node in
  node = new_fs_node(entry, curr_dir_node);

  // Sanity check
  if (node == NULL) {
    fprintf(stderr, "new_file(): couldn't init fs_node!\n");
    return NULL;
  }

  // Attempt to allocate/initialize blocks
  for (unsigned int i = 0; i < num_blocks_needed; i++) {
    // Request a block from disk
    block_index = get_block();

    if (block_index == -1) {
      // No space was left. Delete what was allocated so far
      fprintf(stderr, "new_file(): impossible/irrecoverable error: no space left on disk!\n");
      return NULL;
    }

    // Write each block with 0xAA (easier to debug) and insert index blocks
    if (i == num_blocks_needed - 1) {
      // For the last block, write only up to the offset if nonzero
      if (last_offset != 0) {
        memset(blocks + block_index, 0xAA, last_offset);
      } else {
        memset(blocks + block_index, 0xAA, sizeof(block)); 
      }

      insert_inode(block_index, last_offset, entry);
    } else {
      memset(blocks + block_index, 0xAA, sizeof(block));
      insert_inode(block_index, 0, entry);
    }
  }

  return node;
}

fs_node* new_dir(char* name, user_id user, permissions perms,
  fs_node* curr_dir_node) {
  fs_entry* entry; // Entry for new directory

  // Create a new filesystem entry and node for the dirctory
  entry = new_fs_entry(name, DIRY, user, perms, 0);

  if (entry == NULL) {
    // There was a problem.
    fprintf(stderr, "new_dir(): couldn't make new entry!\n");
    return NULL;
  }

  return new_fs_node(entry, curr_dir_node);
}

void delete_file(fs_node* file_node) {
  // For traversing through index node list
  index_node* inode = file_node->entry->inode_head; 

  // Unlink from parent
  unlink_child(file_node);

  // Free the disk blocks
  while (inode != NULL) {
    // Free this block, i.e. return it to the free list
    free_block(inode->index);

    inode = inode->next;
  } 
  
  // Delete the node
  delete_fs_node(&file_node);
}

// XXX: deleting root leads to undefined behavior!!
void delete_dir(fs_node* dir_node) {
  fs_node* node; // Temp node for traversing; for readability

  // Unlink from parent
  unlink_child(dir_node);

  // Delete each child
  for (unsigned int c = 0; c < dir_node->num_children; c++) {
    node = dir_node->children[c];
    
    // Check if directory
    if (node->entry->type == DIRY) {
      // It's a directory; delete recursively
      delete_dir(node);
    } else {
      delete_file(node);
    }
  }

  // Free this
  delete_fs_node(&dir_node);
}

