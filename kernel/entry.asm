; kernel/entry.asm - 32-bit entry from bootloader
BITS 32
GLOBAL _start
EXTERN kmain

SECTION .text
_start:
    ; We are at 0x8000, already in protected mode, flat segments set.
    call kmain
.hang:
    cli
    hlt
    jmp .hang

SECTION .note.GNU-stack noalloc noexec nowrite

