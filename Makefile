CFLAGS=-m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra -O2
LDFLAGS=-melf_i386
AS=nasm
ASFLAGS=-f bin
LD=ld
CC=gcc
OBJCOPY=objcopy

BUILD=build
DIST=dist
BOOT=boot
KERNEL=kernel
IMG=$(DIST)/minios.img
ISO=$(DIST)/minios.iso

all: $(IMG)

$(BUILD):
	mkdir -p $(BUILD)
	mkdir -p $(DIST)

$(BUILD)/bootloader.bin: $(BOOT)/boot.asm | $(BUILD)
	$(AS) -f bin -o $@ $<

$(BUILD)/kernel_entry.o: $(KERNEL)/entry.asm | $(BUILD)
	nasm -f elf32 -o $@ $<

$(BUILD)/kernel.o: $(KERNEL)/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/vga.o: $(KERNEL)/vga.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/idt.o: $(KERNEL)/idt.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/idt_load.o: $(KERNEL)/idt_load.asm | $(BUILD)
	nasm -f elf32 -o $@ $<

$(BUILD)/isr.o: $(KERNEL)/isr.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/timer.o: $(KERNEL)/timer.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/keyboard.o: $(KERNEL)/keyboard.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/shell.o: $(KERNEL)/shell.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/fs.o: $(KERNEL)/fs.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/lib.o: $(KERNEL)/lib.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(KERNEL) -c -o $@ $<

$(BUILD)/kernel.elf: $(BUILD)/kernel_entry.o $(BUILD)/kernel.o $(BUILD)/vga.o $(BUILD)/idt.o $(BUILD)/idt_load.o $(BUILD)/isr.o $(BUILD)/timer.o $(BUILD)/keyboard.o $(BUILD)/shell.o $(BUILD)/fs.o $(BUILD)/lib.o $(KERNEL)/linker.ld
	$(LD) -m elf_i386 -T $(KERNEL)/linker.ld -o $@ $(BUILD)/kernel_entry.o $(BUILD)/kernel.o $(BUILD)/vga.o $(BUILD)/idt.o $(BUILD)/idt_load.o $(BUILD)/isr.o $(BUILD)/timer.o $(BUILD)/keyboard.o $(BUILD)/shell.o $(BUILD)/fs.o $(BUILD)/lib.o

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(IMG): $(BUILD)/bootloader.bin $(BUILD)/kernel.bin | $(BUILD)
	dd if=/dev/zero of=$@ bs=512 count=6144 status=none
	dd if=$(BUILD)/bootloader.bin of=$@ conv=notrunc status=none
	dd if=$(BUILD)/kernel.bin of=$@ bs=512 seek=1 conv=notrunc status=none

run: $(IMG)
	qemu-system-i386 -drive format=raw,file=$(IMG) -serial stdio -nographic

clean:
	rm -rf $(BUILD) $(DIST)

.PHONY: all run clean

