; kernel/idt_load.asm - helper to load IDT from C
BITS 32
GLOBAL idt_load

idt_load:
    ; args: (void* base, uint16_t size)
    ; build a temporary idtr on stack
    push ebp
    mov ebp, esp
    sub esp, 6
    mov ax, [ebp+12]     ; size
    mov [esp], ax        ; limit
    mov eax, [ebp+8]     ; base
    mov [esp+2], eax     ; base
    lidt [esp]
    mov esp, ebp
    pop ebp
    ret

