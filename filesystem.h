#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define DISK_SIZE 1024
#define FILE_NM_LEN 128

// Stores the file/directory metadata; doesn't actually 'reside on disk' 
typedef struct {
  // TODO:  
} fs_directory;

// File types
typedef enum {
  TXT, // Plaintext
  EXE, // Binary executable
  IMG, // Image
  DOC, // Formatted document
  MOV  // Video
} file_type;

// User IDs
typedef unsigned long user_id;

// Permissions
typedef struct {
  unsigned char read: 1; // 1 if has read permission
  unsigned char write: 1; // 1 if has write permission
} permissions;

typedef struct {
  char name[FILE_NM_LEN]; // Name of the file
  file_type type; // File type
  user_id user; // User ID of owner 
  permissions perms; // Permissions  
} file_entry;

extern unsigned char disk; // The 'disk drive'

#endif // FILESYSTEM_H
