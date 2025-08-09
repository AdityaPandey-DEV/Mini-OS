#include "idt.h"
#include "lib.h"

#define IDT_ENTRIES 256

static struct idt_entry idt[IDT_ENTRIES];

void idt_set_gate(int n, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[n].base_lo = base & 0xFFFF;
    idt[n].sel = sel;
    idt[n].always0 = 0;
    idt[n].flags = flags;
    idt[n].base_hi = (base >> 16) & 0xFFFF;
}

extern void idt_load(void* base, uint16_t size);

void idt_init(void) {
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0x08, 0x8E);
    }
    idt_load(idt, sizeof(idt));
}

