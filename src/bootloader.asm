CYLS    EQU     10

    ORG     0x7c00

    JMP     entry
; Standard FAT12 formate floppy header
    DB      0x90
    DB      "BOOTLOADER"    ; Bootloader name
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
    MOV     AX, 0           ; Init register
    MOV     SS, AX
    MOV     SP, 0x7C00
    MOV     DS, AX

; Load floppy disk

    MOV     AX, 0x0820
    MOV     ES, AX
    MOV     CH, 0           ; Cylinder 0
    MOV     DH, 0           ; Head 0
    MOV     CL, 2           ; Sector 2
readloop:
    MOV     SI, 0           ; Record number of fail
retry:
    MOV     AH, 0x02        ; Read disk cmd
    MOV     AL, 1           ; Read 1 sector
    MOV     BX, 0
    MOV     DL, 0x00        ; Driver A
    INT     0x13            ; Call disk driver BIOS
    JNC     next             ; Successful
    ADD     SI, 1
    CMP     SI, 5
    JAE     error
    MOV     AH, 0x00
    MOV     DL, 0x00
    INT     0x13            ; Driver reset
    JMP     retry
next:
    MOV     AX, ES          ; Address add 0x200
    ADD     AX, 0x0020
    MOV     ES, AX
    ADD     CL, 1           ; Load next sector
    CMP     CL, 18
    JBE     readloop        ; Jump when CL <=18
    MOV     CL, 1
    ADD     CH, 1
    CMP     CH, CYLS
    JB      readloop        ; Jump when CH < CYLS
fin:
    HLT
    JMP     fin

error:
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