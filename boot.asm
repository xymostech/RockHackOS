%define sectors 14

%define SYSSIZE 0x8000
%define BOOTSEG 0x07c0
%define INITSEG 0x9000
%define SYSSEG  0x1000            ; system loaded at 0x10000 (65536).
%define ENDSEG  SYSSEG + SYSSIZE

start:
        mov ax, BOOTSEG
        mov ds, ax
        mov ax, INITSEG
        mov es, ax
        mov cx, 256
        sub si, si
        sub di, di
        rep movsw
        jmp INITSEG:go
go:
        mov ax, cs
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, 0x400        ; arbitrary value >>512

        mov ah, 0x03    ; read cursor pos
        xor bh, bh
        int 0x10

        mov cx, 20
        mov bx, 0x0007    ; page 0, attribute 7 (normal)
        mov bp, intro_string
        mov ax, 0x1301    ; write string, move cursor
        int 0x10

; ok, we've written the message, now
; we want to load the system (at 0x10000)

        mov ax, SYSSEG
        mov es, ax        ; segment of 0x010000

        call read_sectors

; if the read went well we get current cursor position and save it for
; posterity.

        ;mov    ah,0x03    ; read cursor pos
        ;xor    bh,bh
        ;int    0x10        ; save it in known place, con_init fetches
        ;mov    [510],dx    ; it from 0x90510.

        cli            ; no interrupts allowed !

; first we move the system to it's rightful place

        mov ax, 0x0000
        cld
do_move:
        mov es, ax
        add ax, 0x1000
        cmp ax, 0x9000
        jz end_move
        mov ds, ax
        sub di, di
        sub si, si
        mov cx, 0x8000
        rep movsw
        jmp do_move
end_move:

        ;; then we load the segment descriptors

        mov ax,cs
        mov ds,ax
        o32 lidt [idt_48]
        o32 lgdt [gdt_48]

        ;; enable A20

        mov al,0xD1             ; command write
        out 0x64,al

        mov al,0xDF             ; A20 on
        out 0x60,al

        ;; Finally, switch to protected mode and jump to the os

        mov    ax,0x0001        ; protected mode (PE) bit
        lmsw   ax
        jmp    dword 0x8:0x0    ; jmp offset 0 of segment 8 (cs)

        ;; If this goes over 128, we need to make the stuff below better
        ;; This should also be one more than os.bin's size, because it
        ;; includes this file
%define TOTALSECTORS 3
%define SECTORSREAD 18

        ;; The current sector we've read in
sector:
        dw 1
cylinder:
        dw 0

read_sectors:
        ;; Read in all the sectors that we need for the OS
        push ax
        push bx
        push cx
        push dx
.read_loop:
        ;; leave es
        ;; bx = 512 * ([sector] - 1)
        ;; al = [cylinder] == 0 ? SECTORSREAD - 1 : (TOTALSECTORS - [sector] > SECTORSREAD ? SECTORSREAD : TOTALSECTORS - [sector])
        ;; cl = [cylinder] == 0 ? 2 : 1
        ;; ch = [cylinder]

        mov ax, [cylinder]
        test ax, ax
        jz .sector_zero_cylinder
        mov ax, TOTALSECTORS
        sub ax, [sector]
        cmp ax, SECTORSREAD
        jg .sector_normal_cylinder
        jmp .sector_last_cylinder
.sector_zero_cylinder:
        mov ax, SECTORSREAD
        dec ax
        jmp .sector_done
.sector_normal_cylinder:
        mov ax, SECTORSREAD
        jmp .sector_done
.sector_last_cylinder:
.sector_done:

        mov cx, [cylinder]
        test cx, cx
        jz .zero_cylinder2
        jmp .other_cylinder
.zero_cylinder2:
        mov cl, 2
        jmp .cylinder_done
.other_cylinder:
        mov ch, cl
        mov cl, 1
.cylinder_done:

        mov bx, [sector]
        sub bx, 1
        shl bx, 9

        call read_sector

        mov bx, [cylinder]
        inc bx
        mov [cylinder], bx

        mov bx, [sector]
        mov ah, 0
        add bx, ax
        mov [sector], bx
        cmp bx, TOTALSECTORS
        jl .read_loop

        pop dx
        pop cx
        pop bx
        pop ax
        ret

        ;; Read in the next al sectors to es:bx
read_sector:
        push ax
        push dx

        mov dx, 0
        mov ah, 02h
        int 13h

        pop dx
        pop ax
        ret

gdt:
        dw    0,0,0,0        ; dummy

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

idt_48:
    dw    0                     ; idt limit=0
    dw    0,0                   ; idt base=0L

gdt_48:
    dw    0x800                 ; gdt limit=2048, 256 GDT entries
    dw    gdt,0x9               ; gdt base = 0X9xxxx

intro_string:
        db "Loading system ...", 13, 10

footer:
        times 510-($-$$) db 0
        dw 0xAA55               ; The standard PC boot signature
