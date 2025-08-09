#include "idt.h"
#include "lib.h"
#include "vga.h"

extern void __lib_tick_inc(void);

void isr_init(void) {
    // No CPU ISR handlers installed in this minimal build.
}

void irq_install(void) {
    // Do not enable hardware IRQs in this minimal build; rely on polling.
    __asm__ volatile ("cli");
}

void isr_handler(uint32_t int_no, uint32_t err_code) {
    (void)int_no; (void)err_code;
}

void irq_handler(uint32_t irq_no) {
    (void)irq_no;
}

