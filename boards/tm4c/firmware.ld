ENTRY(Reset_Handler)
MEMORY { FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
 RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 32K }
_estack = ORIGIN(RAM)+LENGTH(RAM);
_Min_Stack_Size = 1024;
PHDRS { code PT_LOAD FLAGS(5); data PT_LOAD FLAGS(6); }
SECTIONS {
 .isr_vector : { .  =  ALIGN(4); KEEP(*(.isr_vector)) } > FLASH :code
 .text : { *(.text*) *(.rodata*) KEEP(*(.init)) KEEP(*(.fini)) .  =  ALIGN(4); _etext = .; } > FLASH :code
 .ARM.extab : { *(.ARM.extab*) } > FLASH :code
 .ARM.exidx : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > FLASH :code
 .preinit_array : { PROVIDE_HIDDEN(__preinit_array_start = .); KEEP(*(.preinit_array*)) PROVIDE_HIDDEN(__preinit_array_end = .); } > FLASH :code
 .init_array : { PROVIDE_HIDDEN(__init_array_start = .); KEEP(*(SORT(.init_array.*))) KEEP(*(.init_array*)) PROVIDE_HIDDEN(__init_array_end = .); } > FLASH :code
 .fini_array : { PROVIDE_HIDDEN(__fini_array_start = .); KEEP(*(SORT(.fini_array.*))) KEEP(*(.fini_array*)) PROVIDE_HIDDEN(__fini_array_end = .); } > FLASH :code
 _sidata = LOADADDR(.data);
 .data : { .  =  ALIGN(4); _sdata = .; *(.data*) .  =  ALIGN(4); _edata = .; } > RAM AT> FLASH :data
 .vtable (NOLOAD) : { . = ALIGN(1024); KEEP(*(vtable)) KEEP(*(.vtable)) } > RAM :NONE
 .bss (NOLOAD) : { .  =  ALIGN(8); _sbss = .; *(.bss*) *(COMMON) .  =  ALIGN(8); _ebss = .; } > RAM :NONE
 .heap (NOLOAD) : { _heap_start = .; . +=  1024; _heap_end = .; } > RAM :NONE
 .stack (NOLOAD) : { .  =  ALIGN(8); . +=  _Min_Stack_Size; } > RAM :NONE
 ASSERT(_heap_end + _Min_Stack_Size <= _estack, "RAM heap/stack overflow")
}
