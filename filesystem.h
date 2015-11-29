#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string.h>

#define DISK_SIZE 1024
#define BLOCK_SIZE 64

//
// This file has to do with mostly 'hardware', lower-level details.
//

const unsigned int NUM_BLOCKS = DISK_SIZE/BLOCK_SIZE;

// Stores the file/directory metadata; doesn't actually 'reside on disk' 
typedef struct {
  // TODO:  
} fs_directory;

// An empty block; used to maintain free list of blocks
typedef struct {
  int next; // Index of next block on free list; -1 if none
  unsigned char unused[BLOCK_SIZE - sizeof(int)]; // Padding
} block;

extern unsigned char disk[DISK_SIZE]; // The 'disk drive'
extern unsigned int next_free_block; // Index of the next free block

//
// Functions
//

// Initialize blocks on disk, i.e. the free list
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

// Request a block. Returns the index of the block or -1 if no space is left.
unsigned int get_block();

// Free a block.
void free_block();

#endif // FILESYSTEM_H
