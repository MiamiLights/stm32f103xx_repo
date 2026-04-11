ENTRY(Reset_Handler)
MEMORY{
    FLASH(rx):ORIGIN =0x08000000,LENGTH =128K
    SRAM(rwx):ORIGIN =0x20000000,LENGTH =20K
}
SECTIONS{
    .text :
    {
        . = ALIGN(8);
        *(.isr_vector)
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(8);
        _etext = .;
    }> FLASH

    .data :
    {
        . = ALIGN(8);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(8);
        _edata = .;
    }> SRAM AT> FLASH

    .bss :
    {
        . = ALIGN(8);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(8);
        _ebss = .;
        __bss_end__ = _ebss;
        . = ALIGN(8);
        end = .;
        __end__ = end;
    }>SRAM
}
