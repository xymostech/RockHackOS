        BITS 32

        mov eax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov eax, 0x1000
        mov esp, eax

        call setup_gdt
        call setup_idt

        int 49

        mov dword [0xb8000], '( : '

        jmp $

setup_gdt:
        lgdt [gdt_pointer]
        jmp 0x8:.clear_cs
.clear_cs:
        mov eax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        ret

setup_idt:
        lidt [idt_pointer]
        ret

div_0_int:
        mov dword [0xb8000], ') : '
        hlt

int_49_int:
        mov dword [0xb8000], ') : '
        iret

        times 0x200 - ($ - $$) dw 0
gdt:
        ;; null descriptor
        dw 0,0,0,0

        ;; Code

        dw    0xFFFF        ; 128 Mb
        dw    0x0000        ; base address=0
        dw    0x9A00        ; code read/exec
        dw    0x00C0        ; granularity=4096, 386

        ;; Data

        dw    0xFFFF        ; 128 Mb
        dw    0x0000        ; base address=0
        dw    0x9200        ; data read/write
        dw    0x00C0        ; granularity=4096, 386

gdt_pointer:
        dw $ - gdt - 1
        dd gdt

idt:
        dw div_0_int
        dw 0x8
        dw 0x8E00
        dw 0

        times (48*2) dd 0

        dw int_49_int
        dw 0x8
        dw 0x8E00
        dw 0

idt_pointer:
        dw (50*8)-1
        dd idt
