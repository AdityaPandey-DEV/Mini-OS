ENTRY(_start)
PHDRS {
  text PT_LOAD FLAGS(5); /* R X */
  data PT_LOAD FLAGS(6); /* R W */
}
SECTIONS {
  . = 0x00008000; /* must match boot jump */
  .text : {
    *(.multiboot)
    *(.text*)
  } :text

  .rodata : {
    *(.rodata*)
  } :text

  .data : {
    *(.data*)
  } :data

  .bss : {
    *(.bss*)
    *(COMMON)
  } :data
}

