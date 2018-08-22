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
    mov     AX, 0           ; Init register
    mov     SS, AX
    mov     SP, 0x7C00
    mov     DS, AX

; Load floppy disk

    mov     AX, 0x0820
    mov     ES, AX
    mov     CH, 0           ; Cylinder 0
    mov     DH, 0           ; Head 0
    mov     CL, 2           ; Sector 2
readloop:
    mov     SI, 0           ; Record number of fail
retry:
    mov     AH, 0x02        ; Read disk cmd
    mov     AL, 1           ; Read 1 sector
    mov     BX, 0
    mov     DL, 0x00        ; Driver A
    int     0x13            ; Call disk driver BIOS
    jnc     next             ; Successful
    add     SI, 1
    cmp     SI, 5
    jae     error
    mov     AH, 0x00
    mov     DL, 0x00
    int     0x13            ; Driver reset
    jmp     retry
next:
    mov     AX, ES          ; address add 0x200
    add     AX, 0x0020
    mov     ES, AX
    add     CL, 1           ; Load next sector
    cmp     CL, 18
    jbe     readloop        ; Jump when CL <=18
    mov     CL, 1
    add     DH, 1
    cmp     DH, 2
    jb      readloop
    mov     DH, 0
    add     CH, 1
    cmp     CH, CYLS
    jb      readloop        ; Jump when CH < CYLS

; Turn-off PIC interrupt
    mov     AL, 0xFF
    out     0x21, AL
    nop
    out     0xA1, AL

    cli                 ; Turn-off cli interrupt

; Turn-on A20 gate
    call    waitkbdout
    mov     AL, 0xD1
    out     0x64, AL
    call    waitkbdout
    mov     AL, 0xDF
    out     0x60, AL
    call    waitkbdout

; move code to the memery ater 1MB
    mov     esi, 0xc400
    mov     edi, 0x100000
    mov     ecx, 10*1024
    call    memcpy

; Jump to protect mode
    lgdt    [GDTR0]
    mov     EAX, CR0
    and     EAX, 0x7FFFFFFF
    or      EAX, 0x00000001
    mov     CR0, EAX
;     jmp     pipelineflush
; pipelineflush:
    mov     AX, 2*8
    mov     DS, AX
    mov     ES, AX
    mov     FS, AX
    mov     GS, AX
    mov     SS, AX

; Jump to os
    ; mov     DWORD EAX, [0x00100004]
    jmp     DWORD 1*8:0x00100010

waitkbdout:
    in      AL, 0x64
    and     AL, 0x02
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
    mov     SI, msg
putloop:
    mov     AL, [SI]
    add     SI, 1
    cmp     AL, 0
    je      fin
    mov     AH, 0x0E        ; Show one character
    mov     BX, 15          ; Set font color
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