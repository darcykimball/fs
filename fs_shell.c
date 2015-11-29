#include "filesystem.h"
#include "fscommands.h"
#include "shell.h"

int main() {
  // Initialize disk and filesystem tree
  init_disk();
  
  fs_node* fstree = new_fs_tree();
  curr_dir_node = fstree;

  loop(command_map, NUM_COMMANDS);
}
