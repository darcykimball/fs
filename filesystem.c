#include <stdio.h>
#include "filesystem.h"

int debug = 0; // Debug message flag
unsigned char disk[DISK_SIZE]; // The 'hard disk'
int next_free_block = 0; // Index of next free block
unsigned int num_free_blocks = NUM_BLOCKS; // Number of free blocks

void init_disk() {
  printf("init_disk(): Initializing disk...\n");

  // Wipe the disk and setup free list
  init_disk_blocks();
  next_free_block = 0;
  num_free_blocks = NUM_BLOCKS;
}

void init_disk_blocks() {
  block* curr = (block*)disk; // Current block being initialized

  if (debug) {
    fprintf(stderr, "init_disk_blocks(): Initializing disk blocks...\n");
  }

  // Set each block to point to the next
  for (unsigned int i = 0; i < NUM_BLOCKS - 1; i++) {
    // Initialize rest of block to zero (for debugging; easier to see)
    memset(curr, 0, sizeof(block));
    
    // Set to point to immediate successor
    curr->next = i + 1;

    curr++;
  }

  // Set last block to point to 'null'
  curr->next = -1;
}


int get_block() {
  block* blocks = (block*)disk; // Pointer to disk as blocks
  unsigned int free_index; // Return value

  // Check for space
  if (next_free_block == -1) {
    fprintf(stderr, "get_block(): no space left!\n");

    // Return error
    return -1;
  }

  // Update global trackers
  free_index = next_free_block;
  num_free_blocks--;

  // Check if this was the last block
  if (blocks[next_free_block].next == -1) {
    // Set to -1 to indicate fullness
    next_free_block = -1;
  } else {
    // Unlink head from free list
    next_free_block = blocks[next_free_block].next;
  }

  return free_index; 
}

void free_block(unsigned int block_index) {
  block* blocks = (block*)disk; // Pointer to disk as blocks

  // Zero out for debugging convenience (easier to see when freed)
  memset(blocks + block_index, 0, sizeof(block));

  // Add the block back to the free list, at the head
  blocks[block_index].next = next_free_block;

  // Update global trackers
  next_free_block = block_index;
  num_free_blocks++;
}

void dump_block(unsigned int block_index) {
  block* blocks = (block*)disk; // Pointer to disk as blocks
  unsigned char* block_bytes = (unsigned char*)(blocks + block_index);
  
  printf("Block %u:\n", block_index);

  // Print out a word at a time
  // XXX: this only works if BLOCK_SIZE is a multiple of a word, in bytes!!
  for (unsigned int i = 0; i < BLOCK_SIZE/sizeof(int); i++) {
    for (unsigned int j = 0; j < sizeof(int); j++) {
      printf("%02X", block_bytes[i*sizeof(int) + j]);
    }
    printf("\n");
  }
}
