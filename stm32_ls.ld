ENTRY(Reset_Handler)
MEMORY{
    FLASH(rx):ORIGIN =0x08000000,LENGTH =128K
    SRAM(rwx):ORIGIN =0x20000000,LENGTH =20K
}
SECTIONS{
    .text :
    {
        *(.isr_vector)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        _etext = .;
    }> FLASH

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    }> SRAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
        . = ALIGN(4);
        end = .;
        __end__ = end;
    }>SRAM

    __heap_limit = 0x20004C00;
}
