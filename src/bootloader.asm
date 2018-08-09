CYLS    EQU     10

    ORG     0x7c00

    JMP     entry
; Standard FAT12 formate floppy header
    DB      0x90
    DB      "mkfs.fat"    ; Bootloader name
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
    DD      0            ; Size of rewrite sector
    DB      0, 0, 0x29      ; Unknown
    DD      0x0A313FB7      ;
    DB      "NO NAME    "   ; Disk name (11 bytes)
    DB      "FAT12   "      ; Formation name (8 bytes)
    TIMES   18 DB 0
    ; DW      0x1F0E
    ; DD      0xAC7C5BBE
    ; DD      0x0B74C022
    ; DD      0xBB0EB456
    ; DD      0x10CD0007
    ; DD      0x32F0EB5E
    ; DD      0xCD16CDE4
    ; DB      0x19, 0xEB, 0xFE
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
    ADD     DH, 1
    CMP     DH, 2
    JB      readloop
    MOV     DH, 0
    ADD     CH, 1
    CMP     CH, CYLS
    JB      readloop        ; Jump when CH < CYLS

; Turn-off PIC interrupt
    MOV     AL, 0xFF
    OUT     0x21, AL
    NOP
    OUT     0xA1, AL

    CLI                 ; Turn-off cli interrupt

; Turn-on A20 gate
    CALL    waitkbdout
    MOV     AL, 0xD1
    OUT     0x64, AL
    CALL    waitkbdout
    MOV     AL, 0xDF
    OUT     0x60, AL
    CALL    waitkbdout

; Move code
    mov     esi, 0xc400
    mov     edi, 0x100000
    mov     ecx, 10*1024
    call    memcpy

; Jump to protect mode
; [INSTRSET "1486p"]
    LGDT    [GDTR0]
    MOV     EAX, CR0
    AND     EAX, 0x7FFFFFFF
    OR      EAX, 0x00000001
    MOV     CR0, EAX
;     jmp     pipelineflush
; pipelineflush:
    MOV     AX, 2*8
    MOV     DS, AX
    MOV     ES, AX
    MOV     FS, AX
    MOV     GS, AX
    MOV     SS, AX

; Jump to os
    ; MOV     [0xFF0], CH     ; Record the IPL position
    ; HLT
    MOV     DWORD EAX, [0x00100004]
    JMP     DWORD 1*8:0x00100010

waitkbdout:
    IN      AL, 0x64
    AND     AL, 0x02
    JNZ     waitkbdout
    RET

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
    JMP     fin
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