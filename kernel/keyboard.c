#include "keyboard.h"
#include "lib.h"
#include "shell.h"

static int shift = 0;
static int ctrl = 0;

static const char scancode_set1[128] = {
    0, 27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\','z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_init(void) {
    // unmask IRQ1 already done in PIC remap unmask; nothing else
}

int keyboard_read_char(void) {
    if ((inb(0x64) & 1) == 0) return -1; // no data
    uint8_t sc = inb(0x60);
    if ((sc & 0x80) == 0) {
        uint8_t code = sc;
        char ch = scancode_set1[code];
        if (ch) {
            if (shift && ch >= 'a' && ch <= 'z') ch = (char)toupper(ch);
            return ch;
        }
    } else {
        uint8_t rel = sc & 0x7F;
        (void)rel;
        // modifiers
        if (sc == 0x2A || sc == 0x36) shift = 1;
        if (sc == 0xAA || sc == 0xB6) shift = 0;
        if (sc == 0x1D) ctrl = 1;
        if (sc == 0x9D) ctrl = 0;
    }
    return -1;
}

