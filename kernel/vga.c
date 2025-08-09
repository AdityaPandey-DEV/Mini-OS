/* Very small VGA text output and utilities */
#include "vga.h"
#include "lib.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* const VGA_MEM = (uint16_t*)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0x07; // light grey on black

static void vga_move_hw_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

uint8_t vga_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}

void vga_set_color(uint8_t color) {
    current_color = color;
}

void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEM[y * VGA_WIDTH + x] = (uint16_t)(' ' | (current_color << 8));
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    vga_move_hw_cursor();
}

void vga_init(void) {
    vga_set_color(0x07);
    vga_clear();
}

static void vga_scroll() {
    if (cursor_y < VGA_HEIGHT)
        return;
    // scroll up one line
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_MEM[(y - 1) * VGA_WIDTH + x] = VGA_MEM[y * VGA_WIDTH + x];
        }
    }
    // clear last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_MEM[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)(' ' | (current_color << 8));
    }
    cursor_y = VGA_HEIGHT - 1;
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~3;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_MEM[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)(' ' | (current_color << 8));
        }
    } else {
        VGA_MEM[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)(c | (current_color << 8));
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    vga_scroll();
    vga_move_hw_cursor();
}

void vga_write(const char* s) {
    while (*s) vga_putc(*s++);
}

void vga_write_color(const char* s, uint8_t color) {
    uint8_t old = current_color;
    current_color = color;
    vga_write(s);
    current_color = old;
}

void vga_set_cursor(int x, int y) {
    cursor_x = x; cursor_y = y; vga_move_hw_cursor();
}

void vga_get_cursor(int* x, int* y) {
    if (x) *x = cursor_x;
    if (y) *y = cursor_y;
}

