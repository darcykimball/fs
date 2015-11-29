#ifndef FSCOMMANDS_H
#define FSCOMMANDS_H

#include <stdio.h>
#include <stddef.h>
#include "fstree.h"
#include "shell.h"

#define NUM_COMMANDS 14

extern user_id curr_user; // Current user
extern fs_node* curr_dir_node; // The current directory
extern fs_node* root_node; // Root directory
extern command_pair command_map[NUM_COMMANDS];

// List current directory
void currentd(size_t argc, char** argv);

// Change to a given directory; if no args given, go to root directory
void chdir(size_t argc, char** argv);

void maked(size_t argc, char** argv);

void createf(size_t argc, char** argv);

void extendf(size_t argc, char** argv);

void trncf(size_t argc, char** argv); // 'truncf' is actually a lib function.

void deletefd(size_t argc, char** argv);

void listd(size_t argc, char** argv);

void listf(size_t argc, char** argv);

void sizef(size_t argc, char** argv);

void movf(size_t argc, char** argv);

void listfb(size_t argc, char** argv);

void dumpfs(size_t argc, char** argv);

void formatd(size_t argc, char** argv);

#endif // FSCOMMANDS_H
