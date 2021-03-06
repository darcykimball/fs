#ifndef FSTREE_H
#define FSTREE_H

#define MAX_DIR_FILES 64 // This was arbitrarily chosen
#define MAX_NM_LEN 128 // Max filename length
#define ROOT_USER_ID 0 // ID of root

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
typedef enum {
  READ, // Read permission
  RDWR, // Read/write
  SYML  // Symbolic link!?? FIXME: unused
} permissions;

// An index node; to be used in singly linked list
typedef struct index_node {
  unsigned int index; // Index of block on disk
  unsigned int offset; // Offset into the block for the end of the used portion
                       // A value of 0 means the whole block is used.
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
  index_node* inode_head; // List of indices of blocks used for this file; head
  index_node* inode_tail; // List of indices of blocks used for this file; tail
} fs_entry;


// A node representing something in the filesystem
typedef struct fs_node {
  fs_entry* entry; // Pointer to entry associated with this node
  struct fs_node* children[MAX_DIR_FILES]; // List of children; NULL for regular files
  unsigned int num_children; // Number of children
  struct fs_node* parent; // Parent of this node; makes navigating easier
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

// Make a fresh filesystem tree with a single directory called 'root'.
fs_node* new_fs_tree();

// Insert an index node into an entry's inode list (at tail)
void insert_inode(unsigned int block_index, unsigned int offset,
  fs_entry* entry);

// Remove/delete the tail of an inode list; returns the removed node
index_node* remove_inode_tail(fs_entry* entry);

// Add a child to a directory node; returns -1 for error, 0 otherwise
int insert_child(fs_node* new_child, fs_node* dir);

// Remove a child from its parent node's children list
void unlink_child(fs_node* child);

// Delete the index node list
void delete_inode_list(fs_entry* entry);

//
// Debugging/print functions
//

// Print out the full path ending at the given node
void dump_path(fs_node* node);

// Print out the properties of an entry
void dump_properties(fs_entry* entry);

#endif // FSTREE_H
