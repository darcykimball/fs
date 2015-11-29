#ifndef FSTREE_H
#define FSTREE_H

#define MAX_DIR_FILES 64 // This was arbitrarily chosen
#define MAX_NM_LEN 128 // Max filename length

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

// An index node; to be used in singly linked list
typedef struct index_node {
  unsigned int index; // Index of block on disk
  unsigned int offset; // Offset into the block for the end of the used portion
  struct index_node* next; // Pointer to next on list; NULL if last
} index_node;

// File/directory entry
typedef struct {
  char name[MAX_NM_LEN]; // Name of the file/dir
  file_type type; // File type
  user_id user; // User ID of owner 
  permissions perms; // Permissions  
  unsigned int size_blocks; // Number of blocks used
  unsigned int size_bytes; // Number of bytes used
  index_node* inodes; // List of indices of blocks used for this file
} fs_entry;


// A node representing something in the filesystem
typedef struct fs_node {
  fs_entry* entry; // Pointer to entry associated with this node
  struct fs_node* children[MAX_DIR_FILES]; // List of children; NULL for regular files
  unsigned int num_children; // Number of children
} fs_node;

//
// Constructors/destructors. These functions only set up objects pertaining
// to the directory structure; they do *not* actually modify the 'disk' in
// any way. The callers of these functions must do that.
//

// Construct a new filesystem entry; returns NULL if unsuccessful. Caller
// must make sure the file is actually created, and the inodes are initialized.
fs_entry* new_fs_entry(char* name, file_type type, user_id user,
  permissions perms, unsigned int size_bytes);

// Construct a new filesystem node with no children; returns NULL if
// unsuccessful. Links the new node into the parent's children list.
fs_node* new_fs_node(fs_entry* entry, fs_node* parent);

// Delete a filesystem node; deletes all its associated children. Again, the
// caller must make sure that the actual files on disk are deleted (blocks
// freed, etc.).
void delete_fs_node(fs_node** nodepp);

#endif // FSTREE_H
