        BITS 32

EXTERN _main
EXTERN _keyboard_int
EXTERN _irq6_handler

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

        jmp _main

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

gpf_int:
        mov dword [0xb8006], 'g p '
        hlt

keyboard_int:
        push eax
        in al, 0x60

        push eax
        call _keyboard_int
        pop eax

        mov al, 0x20
        out 0x20, al
        pop eax
        iret

floppy_int:
        push eax
        call _irq6_handler

        mov al, 0x20
        out 0x20, al

        pop eax
        iret

int_48_int:
        mov dword [0xb8000], ') : '
        iret
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
        dw div_0_int            ; divide by 0
        dw 0x8
        dw 0x8E00
        dw 0

        dd 0, 0                 ; debug
        dd 0, 0                 ; nmi
        dd 0, 0                 ; breakpoint
        dd 0, 0                 ; overflow
        dd 0, 0                 ; bound range exceeded
        dd 0, 0                 ; invalid opcode
        dd 0, 0                 ; device not available
        dd 0, 0                 ; double fault
        dd 0, 0                 ; cso?
        dd 0, 0                 ; invalid tss
        dd 0, 0                 ; segment not present
        dd 0, 0                 ; stack segfault

        dw gpf_int                 ; gpf
        dw 0x8
        dw 0x8E00
        dw 0

        dd 0, 0                 ; page fault
        dd 0, 0                 ; reserved
        dd 0, 0                 ; x87 floating point exception
        dd 0, 0                 ; alignment check
        dd 0, 0                 ; machine check
        dd 0, 0                 ; simd floating point exception
        dd 0, 0                 ; virtualization exception
        dd 0, 0                 ; reserved
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0
        dd 0, 0                 ; IRQ 0

        dw keyboard_int         ; IRQ 1 (keyboard)
        dw 0x8
        dw 0x8E00
        dw 0

        dd 0, 0                 ; IRQ 2
        dd 0, 0                 ; IRQ 3
        dd 0, 0                 ; IRQ 4
        dd 0, 0                 ; IRQ 5

        dw floppy_int           ; IRQ 6 (floppy)
        dw 0x8
        dw 0x8E00
        dw 0

        dd 0, 0                 ; IRQ 7
        dd 0, 0                 ; IRQ 8
        dd 0, 0                 ; IRQ 9
        dd 0, 0                 ; IRQ 10
        dd 0, 0                 ; IRQ 11
        dd 0, 0                 ; IRQ 12
        dd 0, 0                 ; IRQ 13
        dd 0, 0                 ; IRQ 14
        dd 0, 0                 ; IRQ 15

        dw int_48_int
        dw 0x8
        dw 0x8E00
        dw 0

idt_pointer:
        dw (49*8)-1
        dd idt
