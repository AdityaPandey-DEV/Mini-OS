/* Minimal C kernel that sets up IDT/PIC, timer, keyboard, and shell */
#include "vga.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "shell.h"
#include "lib.h"

static const char* OS_NAME = "MiniOS";
static const char* OS_VER = "0.1";

void kmain(void) {
    vga_init();
    vga_set_color(vga_color(0xA /* light green */, 0x0));
    vga_write("\\n");
    vga_write(" ");
    vga_write(OS_NAME);
    vga_write(" v");
    vga_write(OS_VER);
    vga_write(" — Welcome!\\n\\n");
    vga_set_color(vga_color(0x7, 0));

    idt_init();
    isr_init();
    irq_install();

    timer_init(100); // 100 Hz
    keyboard_init();

    shell_init();

    // Main loop: poll keyboard into shell
    while (1) {
        int ch = keyboard_read_char();
        if (ch != -1) {
            shell_input((char)ch);
        }
    }
}

