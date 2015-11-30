#include "filesystem.h"
#include "fscommands.h"
#include "shell.h"

int main() {
  // Initialize disk and filesystem tree
  init_disk();
  
  root_node = new_fs_tree();
  curr_dir_node = root_node;
  curr_user = 1;

  printf("You start as user: %d\n", curr_user);

  loop(command_map, NUM_COMMANDS);

  // Cleanup
  delete_fs_node(&root_node);
}
