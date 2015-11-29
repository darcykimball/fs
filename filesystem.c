#include <stdio.h>
#include "filesystem.h"

unsigned char disk[DISK_SIZE]; // The 'hard disk'
unsigned int next_free_block = 0; // Index of next free block

void init_disk() {
  // Wipe the disk and setup free list
  init_disk_blocks();
  next_free_block = 0;
}

void init_disk_blocks() {
  block* curr = (block*)disk; // Current block being initialized

  // Set each block to point to the next
  for (unsigned int i = 0; i < NUM_BLOCKS - 1; i++) {
    // Set to point to immediate successor
    curr->next = i + 1;

    // Initialize rest of block to zero (for debugging; easier to see)
    memset(curr, 0, sizeof(block));

    curr++;
  }

  // Set last block to point to 'null'
  curr->next = -1;
}


int get_block() {
  block* blocks = (block*)disk; // Pointer to disk as blocks
  unsigned int free_index; // Return value

  // Check for space
  if (blocks[next_free_block].next == -1) {
    fprintf(stderr, "get_block(): no space left!\n");

    // Return error
    return -1;
  }

  // Unlink head from free list
  free_index = next_free_block;
  next_free_block = blocks[next_free_block].next;

  return free_index; 
}

void free_block(unsigned int block_index) {
  block* blocks = (block*)disk; // Pointer to disk as blocks

  // Zero out for debugging convenience (easier to see when freed)
  memset(blocks + block_index, 0, sizeof(block));

  // Add the block back to the free list, at the head
  blocks[block_index].next = next_free_block;
  next_free_block = block_index;
}
