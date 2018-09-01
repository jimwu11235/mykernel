CYLS    EQU     10

    ORG     0x7c00

    jmp     entry
; Standard FAT12 formate floppy header
    nop
    DB      "mkfs.fat"      ; Bootloader name
    DW      512             ; Size of sector
    DB      1               ; Size of cluster
    DW      1               ; Start position of FAT (from first sector)
    DB      2               ; Number of FAT (must be 2)
    DW      224             ; Size of root directory
    DW      2880            ; Number of sector
    DB      0xf0            ; Class of floppy disk (must be 0xf0)
    DW      9               ; Length of FAT (must be 9 sectors)
    DW      18              ; Number of sectors per track (must be 18)
    DW      2               ; Number of head (must be 2)
    DD      0               ; Unuesed partition (must be 0)
    DD      0               ; Size of rewrite sector
    DB      0, 0, 0x29      ; Unknown
    DD      0x00000000      ;
    DB      "NO NAME    "   ; Disk name (11 bytes)
    DB      "FAT12   "      ; Formation name (8 bytes)
    TIMES   18 DB 0
; Program start

entry:
    mov     ax, 0           ; Init register
    mov     ss, ax
    mov     SP, 0x7C00
    mov     ds, ax

;     mov     cl, 8
;     mov     ch, 0
; putloop2:
;     sub     cl, 1
;     add     ch, 1
;     cmp     ch, 8
;     ja      fin2
;     mov     al, dl
;     mov     ah, 1
;     sal     ah, cl
;     and     al, ah
;     cmp     al, 0
;     je      put0
;     mov     al, 0x31
;     mov     ah, 0x0E        ; Show one character
;     mov     bx, 15          ; Set font color
;     int     0x10            ; Call VGA BIOS
;     ; jmp     fin2
;     jmp     putloop2
; put0:
;     mov     al, 0x30
;     mov     ah, 0x0E        ; Show one character
;     mov     bx, 15          ; Set font color
;     int     0x10            ; Call VGA BIOS
;     ; jmp     fin2
;     jmp     putloop2
; fin2:
;     jmp fin2

; Load floppy disk

    mov     ax, 0x0820
    mov     es, ax
    mov     ch, 0           ; Cylinder 0
    mov     dh, 0           ; Head 0
    mov     cl, 2           ; Sector 2
readloop:
    mov     si, 0           ; Record number of fail
retry:
    mov     ah, 0x02        ; Read disk cmd
    mov     al, 1           ; Read 1 sector
    mov     bx, 0
    mov     dl, 0x00        ; Driver A
    int     0x13            ; Call disk driver BIOS
    jnc     next             ; Successful
    add     si, 1
    cmp     si, 5
    jae     error
    mov     ah, 0x00
    mov     dl, 0x00
    int     0x13            ; Driver reset
    jmp     retry
next:
    mov     ax, es          ; address add 0x200
    add     ax, 0x0020
    mov     es, ax
    add     cl, 1           ; Load next sector
    cmp     cl, 18
    jbe     readloop        ; Jump when cl <=18
    mov     cl, 1
    add     dh, 1
    cmp     dh, 2
    jb      readloop
    mov     dh, 0
    add     ch, 1
    cmp     ch, CYLS
    jb      readloop        ; Jump when ch < CYLS

; Turn-off PIC interrupt
    mov     al, 0xFF
    out     0x21, al
    nop
    out     0xA1, al

    cli                 ; Turn-off cli interrupt

; Turn-on A20 gate
    call    waitkbdout
    mov     al, 0xD1
    out     0x64, al
    call    waitkbdout
    mov     al, 0xDF
    out     0x60, al
    call    waitkbdout

; Jump to protect mode
    lgdt    [GDTR0]
    mov     eax, cr0
    and     eax, 0x7FFFFFFF
    or      eax, 0x00000001
    mov     cr0, eax
    jmp     pipelineflush
pipelineflush:
    mov     ax, 2*8
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

; move code to the memery after 1MB(must operate after entering protection mode)
    mov     esi, 0xc400
    mov     edi, 0x280000
    mov     ecx, 512*1024
    call    memcpy

; Jump to os
    jmp     DWORD 1*8:0x00280000

waitkbdout:
    in      al, 0x64
    and     al, 0x02
    jnz     waitkbdout
    ret

memcpy:
    mov     eax, [esi]
    add     esi, 4
    mov     [edi], eax
    add     edi, 4
    sub     ecx, 4
    jnz     memcpy
    ret

GDT0:
    TIMES   8 DB 0
    DW      0xFFFF, 0x0000, 0x9A00, 0x00CF  ; Code section
    DW      0xFFFF, 0x0000, 0x9200, 0x00CF  ; Data section

GDTR0:
    DW      8*3-1
    DD      GDT0

error:
    mov     si, msg
putloop:
    mov     al, [si]
    add     si, 1
    cmp     al, 0
    je      fin
    mov     ah, 0x0E        ; Show one character
    mov     bx, 15          ; Set font color
    int     0x10            ; Call VGA BIOS
    jmp     putloop
fin:
    hlt
    jmp     fin
msg:
    DB      0x0a, 0x0a
    DB      "load error"
    DB      0x0a
    DB      0

    ;RESB    0x7DFE-$
    TIMES   510-($-$$) DB 0

    DB      0X55, 0XAA

    ;TIMES   512*(2880-1) DB 0
    DB      0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00
    TIMES   4600 DB 0
    TIMES   1469440 DB 0