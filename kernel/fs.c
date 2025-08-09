#include "fs.h"
#include "lib.h"

static const char file_about[] = "MiniOS kernel demo files.\nBuilt for web terminal.\n";
static const char file_license[] = "MIT License\n";
static const char file_readme[] = "Welcome to MiniOS! Type 'help' to get started.\n";

static const struct file files[] = {
    {"ABOUT.txt", file_about},
    {"LICENSE.txt", file_license},
    {"README.txt", file_readme},
};

const struct file* fs_list(size_t* count) {
    if (count) *count = sizeof(files)/sizeof(files[0]);
    return files;
}

const char* fs_read(const char* name) {
    for (size_t i = 0; i < sizeof(files)/sizeof(files[0]); i++) {
        if (strcmp(files[i].name, name) == 0) return files[i].data;
    }
    return 0;
}

