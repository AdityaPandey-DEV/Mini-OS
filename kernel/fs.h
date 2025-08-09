#pragma once
#include <stdint.h>

// Tiny in-memory FS for demo ls/cat
struct file { const char* name; const char* data; };
const struct file* fs_list(size_t* count);
const char* fs_read(const char* name);

