#ifndef FSTREE_H
#define FSTREE_H

#define MAX_DIR_FILES 64 // This was arbitrarily chosen
#define FILE_NM_LEN 128 // Max filename length

//
// The filesystem directory, as represented as nodes with multiple children.
// Nodes can be either files or directories.
//

//
// Metadata PODs
//

// File types
typedef enum {
  TXT, // Plaintext
  EXE, // Binary executable
  IMG, // Image
  DOC, // Formatted document
  MOV, // Video
  DIRY, // Directory
} file_type;

// User IDs
typedef unsigned int user_id;

// Permissions
typedef struct {
  unsigned char read: 1; // 1 if has read permission
  unsigned char write: 1; // 1 if has write permission
} permissions;

// File/directory entry
typedef struct {
  char name[FILE_NM_LEN]; // Name of the file/dir
  file_type type; // File type
  user_id user; // User ID of owner 
  permissions perms; // Permissions  
  unsigned int size_blocks; // Number of blocks used
  unsigned int size_bytes; // Number of bytes used
} fs_entry;

// A node representing something in the filesystem
typedef struct fs_node {
  fs_entry entry; // The entry associated with this node
  struct fs_node* children[MAX_DIR_FILES]; // List of children; NULL for regular files
  unsigned int num_children; // Number of children
} fs_node;

//
// Constructors
//

// Construct a new filesystem entry; returns NULL if unsuccessful
// This will create a new regular file if 'type' is not DIRE, with size
// size_bytes
fs_entry* new_fs_entry(char* name, file_type type, user_id user,
  permissions perms, unsigned int size_bytes);

// Construct a new filesystem node with no children; returns NULL if unsuccessful
fs_node* new_fs_node(fs_entry* entry, fs_node* parent);

// Delete a filesystem node; deletes all its associated children
void delete_fs_node(fs_node** nodepp);

#endif // FSTREE_H
