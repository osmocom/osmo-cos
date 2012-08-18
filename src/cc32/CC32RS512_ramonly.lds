/* Memory Definitions */


OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(exceptions)

MEMORY
{
/*  FLASH  (rx) : ORIGIN = 0x00000000, LENGTH = 0x00080000 */
  RAM    (rw) : ORIGIN = 0x000C0000, LENGTH = 0x00004000
  STACK  (rw) : ORIGIN = 0x000C4000, LENGTH = 0x00000000
}

/* Section Definitions */

SECTIONS
{
	.text.exceptions : {
		PROVIDE(exceptions = .);
		KEEP(*(.text.exceptions))
		*(.text.exceptions)
	} > RAM

	/* first section is .text which is used for code */
	.text : {
		* (.text)
		* (.glue_7)
		* (.glue_7t)
  		. = ALIGN(4);
  	} >RAM
	PROVIDE(_text_start = ADDR(.text));
	PROVIDE(_text_end = ADDR(.text) + SIZEOF(.text));

	.ctors : {
		LONG(SIZEOF(.ctors) / 4 - 2)
		KEEP(*(SORT(.ctors)))
		LONG(0)
	} >RAM
	PROVIDE(_ctor_start = LOADADDR(.ctors));
	PROVIDE(_ctor_end = LOADADDR(.ctors) + SIZEOF(.ctors));

	.dtors : {
		LONG(SIZEOF(.dtors) / 4 - 2)
		KEEP(*(SORT(.dtors)))
		LONG(0)
	} >RAM
	PROVIDE(_dtor_start = LOADADDR(.dtors));
	PROVIDE(_dtor_end = LOADADDR(.dtors) + SIZEOF(.dtors));

	.rodata : {
		* (.rodata*)
	} > RAM
	PROVIDE(_rodata_start = ADDR(.rodata));
	PROVIDE(_rodata_end = ADDR(.rodata) + SIZEOF(.rodata));

	.got : {
		. = ALIGN(4);
		*(.got)
		*(.got.plt) *(.igot.plt) *(.got) *(.igot)
		. = ALIGN(4);
	} > RAM
	PROVIDE(_got_start = ADDR(.got));
	PROVIDE(_got_end = ADDR(.got) + SIZEOF(.got));

	.data : {
  		_data = . ;
		* (.vectram)
  		* (.data)
  		. = ALIGN(4);
  	} > RAM
	PROVIDE(__data_start = LOADADDR(.data));
	PROVIDE(__data_end = LOADADDR(.data) + SIZEOF(.data));
	PROVIDE(_data_start = ADDR(.data));
	PROVIDE(_data_end = ADDR(.data) + SIZEOF(.data));

	/* .bss section which is used for uninitialized data */
	.bss (NOLOAD) : {
		. = ALIGN(4);
		*(.bss)
		. = ALIGN(4);
	} > RAM
	PROVIDE(__bss_start = ADDR(.bss));
	PROVIDE(__bss_end = ADDR(.bss) + SIZEOF(.bss));
	PROVIDE(_bss_start = __bss_start);
	PROVIDE(_bss_end = __bss_end);

	. = ALIGN(4);
	_end = .;
	PROVIDE(end = .);

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

}
