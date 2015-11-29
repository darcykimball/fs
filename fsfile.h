#ifndef FSFILE_H
#define FSFILE_H

#include "fstree.h"

//
// (Helper) functions for actually creating files, i.e. using disk calls
// and doing the dirty work of updating the directory/file tree details.
// 

// Make a new file of a certain size. Returns NULL if no space is available.
fs_node* new_file(char* name, file_type type, user_id user, permissions perms,
  unsigned int size_bytes, fs_node* curr_dir_node);


// Make a new directory; returns NULL if something went wrong.
fs_node* new_dir(char* name, user_id user, permissions perms,
  fs_node* curr_dir_node);

// Delete a regular file (on disk)
void delete_file(fs_node* file_node);

// Delete a directory and its contents recursively (on disk)
void delete_dir(fs_node* dir_node);

#endif // FSFILE_H
