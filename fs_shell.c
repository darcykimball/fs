#include "filesystem.h"
#include "fscommands.h"
#include "shell.h"

int main() {
  // Set debug on
  debug = 1;

  // Initialize disk and filesystem tree
  init_disk();
  
  root_node = new_fs_tree();
  curr_dir_node = root_node;

  loop(command_map, NUM_COMMANDS);
}
