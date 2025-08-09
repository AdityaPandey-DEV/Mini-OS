#pragma once
#include <stdint.h>
#include <stddef.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void io_wait(void);

void memset(void* dst, int c, size_t n);
void memcpy(void* dst, const void* src, size_t n);
size_t strlen(const char* s);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t n);
char* strcpy(char* dst, const char* src);
char* strcat(char* dst, const char* src);
int isspace(int c);
int isdigit(int c);
int isalpha(int c);
int tolower(int c);
int toupper(int c);

uint32_t ticks(void);
void panic(const char* msg);

