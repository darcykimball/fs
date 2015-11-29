#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string.h>

#define DISK_SIZE 512
#define BLOCK_SIZE 32
#define NUM_BLOCKS (DISK_SIZE/BLOCK_SIZE)

extern int debug; // Debug message flag

//
// This file has to do with mostly 'hardware', lower-level details.
//

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
extern int next_free_block; // Index of the next free block (head of
                                     // free list

//
// Functions
//

// Initialize a new disk
void init_disk();

// Initialize blocks on disk, i.e. the free list
void init_disk_blocks();

// Request a block. Returns the index of the block or -1 if no space is left.
int get_block();

// Free a block.
void free_block(unsigned int block_index);

// Print out the contents of a block
void dump_block(unsigned int block_index);

#endif // FILESYSTEM_H
