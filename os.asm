        BITS 32

EXTERN test_write

        mov eax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        call test_write
        jmp $
