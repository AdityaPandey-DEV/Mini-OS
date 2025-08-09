#pragma once
#include <stddef.h>
#include <stdint.h>

void vga_init(void);
void vga_clear(void);
void vga_putc(char c);
void vga_write(const char* s);
void vga_write_color(const char* s, uint8_t color);
void vga_set_color(uint8_t color);
void vga_set_cursor(int x, int y);
void vga_get_cursor(int* x, int* y);

uint8_t vga_color(uint8_t fg, uint8_t bg);

