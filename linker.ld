ENTRY(_start)

SECTIONS {
  /* Start at 0x7c00 */
  . = 0x7c00;

  /* Text */
  .text : {
    *(.text)
  }

  /* Read-write data (initialized) */
  .data BLOCK(4K) : ALIGN(4K) {
    *(.data)
  }

  /* Read-write data (uninitialized) */
  .bss BLOCK(4K) : ALIGN(4K) {
    *(COMMON)
    *(.bss)
  }

  /* Stack separate */
  .stack BLOCK(4K) : ALIGN(4K) {
    *(.bootstrap_stack)
  }

  /* Read-only data */
  .rodata BLOCK(4K) : ALIGN(4K) {
    *(.rodata)
  }

  /* Other sections will be here */
}
