ENTRY(_start)
SECTIONS {
  . = 0x00008000; /* must match boot jump */
  .text : { *(.multiboot) *(.text*) }
  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }
}

