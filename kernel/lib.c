#include "lib.h"

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void io_wait(void) {
    outb(0x80, 0);
}

void memset(void* dst, int c, size_t n) {
    uint8_t* p = (uint8_t*)dst;
    while (n--) *p++ = (uint8_t)c;
}

void memcpy(void* dst, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while (n--) *d++ = *s++;
}

size_t strlen(const char* s) {
    size_t n=0; while (s[n]) n++; return n;
}

int strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (int)((unsigned char)*a) - (int)((unsigned char)*b);
}

int strncmp(const char* a, const char* b, size_t n) {
    while (n && *a && (*a == *b)) { a++; b++; n--; }
    if (n==0) return 0; return (int)((unsigned char)*a) - (int)((unsigned char)*b);
}

char* strcpy(char* dst, const char* src) {
    char* d = dst; while ((*d++ = *src++)); return dst;
}

char* strcat(char* dst, const char* src) {
    char* d = dst; while (*d) d++; while ((*d++ = *src++)); return dst;
}

int isspace(int c) { return c==' '||c=='\t'||c=='\n'||c=='\r'||c=='\v'||c=='\f'; }
int isdigit(int c) { return c>='0'&&c<='9'; }
int isalpha(int c) { return (c>='a'&&c<='z')||(c>='A'&&c<='Z'); }
int tolower(int c) { return (c>='A'&&c<='Z')?c-('A'-'a'):c; }
int toupper(int c) { return (c>='a'&&c<='z')?c+('A'-'a'):c; }

static volatile uint32_t g_ticks = 0;
uint32_t ticks(void) { return g_ticks; }

void panic(const char* msg) {
    extern void vga_set_color(uint8_t);
    extern void vga_clear(void);
    extern void vga_write(const char*);
    vga_set_color(0x4F); // white on red
    vga_clear();
    vga_write("KERNEL PANIC\n\n");
    vga_set_color(0x4E);
    vga_write(msg);
    for(;;) { __asm__ volatile("cli; hlt"); }
}

// forward decl, increment ticks from timer IRQ handler
void __lib_tick_inc(void) { g_ticks++; }

