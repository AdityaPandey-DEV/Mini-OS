; boot/boot.asm - 512-byte boot sector that loads kernel at LBA 1
; Assembled with: nasm -f bin -o build/bootloader.bin boot/boot.asm
BITS 16
ORG 0x7C00

start:
    cli
    mov [BOOT_DRIVE], dl
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Clear screen (BIOS teletype)
    mov ax, 0x0600
    mov bh, 0x07
    mov cx, 0x0000
    mov dx, 0x184F
    int 0x10

    mov si, banner
.print_banner:
    lodsb
    or al, al
    jz .banner_done
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07
    int 0x10
    jmp .print_banner
.banner_done:

    ; Load kernel (we assume contiguous sectors starting at LBA=1)
    mov si, loadmsg
    call print

    ; Use BIOS INT 13h extensions (LBA) if available, else CHS fallback
    mov bx, 0x8000          ; ES:BX buffer (0x0000:0x8000 => 0x8000)
    mov es, bx
    xor bx, bx              ; BX=0 in ES:BX => ES=0x8000, BX=0

    mov di, DAP
    mov word [di], 0x0010   ; size of DAP
    mov word [di+2], 1      ; number of sectors to read per call
    mov dword [di+4], 0x00008000 ; buffer (segment:offset packed after int13x)
    mov dword [di+8], 1     ; starting LBA = 1

    mov cx, 64              ; read up to 64 sectors (~32 KiB kernel max here)
.read_loop:
    push cx
    mov ah, 0x42
    mov dl, [BOOT_DRIVE]
    mov si, DAP
    int 0x13
    jc disk_error
    ; advance LBA and buffer
    add dword [DAP+8], 1
    add dword [DAP+4], 512
    pop cx
    loop .read_loop

    ; Enable A20
    call enable_a20

    ; Enter protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEL:protected_entry

[BITS 32]
protected_entry:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Jump to kernel at 0x00100000? Here we loaded at 0x00008000; jump there.
    ; We'll have a 32-bit entry in kernel entry.asm that relocates as needed.
    jmp 0x00008000

; ------- 16-bit helpers -------
[BITS 16]
print:
    pusha
.print_loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    mov bl, 0x07
    int 0x10
    jmp .print_loop
.done:
    popa
    ret

disk_error:
    mov si, errmsg
    call print
.hang:
    hlt
    jmp .hang

; Enable A20 via keyboard controller (simple, naive)
enable_a20:
    in al, 0x64
.a20w1:
    test al, 2
    jnz .a20w1
    mov al, 0xD1
    out 0x64, al
.a20w2:
    in al, 0x64
    test al, 2
    jnz .a20w2
    mov al, 0xDF
    out 0x60, al
    ret

banner db "MiniOS v0.1 booting...", 0x0D, 0x0A, 0
loadmsg db "Loading kernel...",0x0D,0x0A,0
errmsg  db "Disk read error!",0x0D,0x0A,0

BOOT_DRIVE db 0

DAP times 16 db 0

; 32-bit flat GDT
[BITS 16]
gdt_start:
    dq 0x0000000000000000 ; null
    dq 0x00CF9A000000FFFF ; code 0x08 base 0 limit 4GB
    dq 0x00CF92000000FFFF ; data 0x10 base 0 limit 4GB

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt_end:

CODE_SEL equ 0x08
DATA_SEL equ 0x10

; Boot sector signature
TIMES 510-($-$$) db 0
DW 0xAA55

; BIOS passes boot drive in DL; store it
%if 0
; (Note: In some BIOSes, DL is preserved. If not, could hook earlier.)
%endif

start_post:
    ret

