ENTRY(start)
SECTIONS
{
  . = 0x280000;

  PROVIDE(kernel_start = .);
  .text :
  {
    *(.text)
    . = ALIGN(4096);
  }
  .data :
  {
    *(.data)
    *(.rodata)
    . = ALIGN(4096);
  }
  .bss :
  {
    *(.bss)
    . = ALIGN(4096);
  }
  .stab :
  {
    *(.stab)
    . = ALIGN(4096);
  }
  .stabstr :
  {
    *(.stabstr)
    . = ALIGN(4096);
  }
  PROVIDE(kernel_end = .);

  /DISCARD/ : { *(.comment) *(.eh_frame) }
}