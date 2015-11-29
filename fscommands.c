#include <stddef.h>
#include "fscommands.h"

// FIXME
fs_node* curr_dir_node = NULL;

command_pair command_map[] = {
  { "currentd", currentd },
  { "chdir", chdir },
  { "maked", maked },
  { "createf", createf },
  { "extendf", extendf },
  { "trncf", trncf },
  { "deletefd", deletefd },
  { "listd", listd },
  { "listf", listf },
  { "sizef", sizef },
  { "movf", movf },
  { "listfb", listfb },
  { "dumpfs", dumpfs },
  { "formatd", formatd }
};

// List current directory
void currentd(size_t argc, char** argv) {
  // Check for valid args
  if (argc > 1) {
    fprintf(stderr, "currentd takes no arguments\n");
    return;
  } 

  (void)argv; // Avoid compiler warning...

  // Sanity heck
  if (curr_dir_node == NULL || curr_dir_node->entry->type != DIRY) {
    fprintf(stderr, "currentd(): NULL or non-directory curr_dir_node!!\n");
    return;
  }

  // Print out the name
  printf("%s\n", curr_dir_node->entry->name);

}

void chdir(size_t argc, char** argv) {
}

void maked(size_t argc, char** argv) {
}

void createf(size_t argc, char** argv) {
}

void extendf(size_t argc, char** argv) {
}

void trncf(size_t argc, char** argv) {

}

void deletefd(size_t argc, char** argv) {
}

void listd(size_t argc, char** argv) {
}

void listf(size_t argc, char** argv) {
}

void sizef(size_t argc, char** argv) {
}

void movf(size_t argc, char** argv) {
}

void listfb(size_t argc, char** argv) {}
void dumpfs(size_t argc, char** argv) {}

void formatd(size_t argc, char** argv) {

}
