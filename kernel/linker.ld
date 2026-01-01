ENTRY(_start)

SECTIONS
{
  /* Match whatever physical/virtual address your bootloader jumps to.
     1 MiB is common, but YOU must align this with your loader. */
  . = 1M;

  .text : ALIGN(4K)
  {
    *(.text*)
  }

  .rodata : ALIGN(4K)
  {
    *(.rodata*)
  }

  .data : ALIGN(4K)
  {
    *(.data*)
  }

  .bss : ALIGN(4K)
  {
    *(COMMON)
    *(.bss*)
  }
}