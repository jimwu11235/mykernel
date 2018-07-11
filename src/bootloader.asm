    ORG     0x7c00

    JMP     entry
; Standard FAT12 formate floppy header
    DB      0x90
    DB      "BOOTLOADER"     ; Bootloader name
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
    DD      2880            ; Size of rewrite sector
    DB      0, 0, 0x29      ; Unknown
    DD      0xFFFFFFFF      ;
    DB      "MYKERNEL   "   ; Disk name (11 bytes)
    DB      "FAT12   "      ; Formation name (8 bytes)
    ;RESB    18              ; 
    TIMES   18 DB 0

; Program start

entry:
    MOV     AX, 0
    MOV     SS, AX
    MOV     SP, 0x7C00
    MOV     DS, AX
    MOV     ES, AX

    MOV     SI, msg
putloop:
    MOV     AL, [SI]
    ADD     SI, 1
    CMP     AL, 0
    JE      fin
    MOV     AH, 0x0E        ; Show one character
    MOV     BX, 15          ; Set font color
    INT     0x10            ; Call VGA BIOS
    JMP     putloop
fin:
    HLT
    JMP putloop

msg:
    DB      0x0a, 0x0a
    DB      "Hello, world"
    DB      0x0a
    DB      0

    ;RESB    0x7DFE-$
    TIMES   510-($-$$) DB 0

    DB      0X55, 0XAA