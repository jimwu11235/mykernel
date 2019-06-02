GLOBAL io_hlt
GLOBAL io_cli, io_sti
GLOBAL io_load_eflags, io_store_eflags
GLOBAL load_cr0, store_cr0
GLOBAL load_tr
GLOBAL memtest_asm
GLOBAL task_switch_3, task_switch_4

[SECTION .text]
io_hlt:             ; void io_hlt(void);
    hlt
    ret

io_cli:             ; void io_cli(void);
    cli
    ret

io_sti:             ; void io_sti(void);
    sti
    ret

io_load_eflags:     ; int io_load_eflags(void);
    pushfd
    pop     eax
    ret

io_store_eflags:    ; void io_store_eflags(int eflags);
    mov     eax, [esp+4]
    push    eax
    popfd
    ret

load_cr0:           ; int load_cr0(void);
    mov     eax, cr0
    ret

store_cr0:          ; void store_cr0(int cr0);
    mov     eax, [esp+4]
    mov     cr0, eax
    ret

load_tr:            ; void load_tr(int tr);
    ltr     [esp+4]
    ret

memtest_asm:        ; unsigned int memtest_asm(unsigned int start, unsigned int end);
    push    edi
    push    esi
    push    ebx
    mov     esi, 0xAA55AA55         ; pat0 = 0xAA55AA55;
    mov     edi, 0x55AA55AA         ; pat1 = 0x55AA55AA;
    mov     eax, [esp+12+4]         ; i = start;
mts_loop:
    mov     ebx, eax
    add     ebx, 0xFFC              ; p = i + 0xFFC;
    mov     edx, [ebx]              ; old = *p;
    mov     [ebx], esi              ; *p = pat0;
    xor     dword [ebx], 0xFFFFFFFF ; *p ^= 0xFFFFFFFF;
    cmp     edi, [ebx]              ; if(*p != pat1)
    jne     mts_fin
    xor     dword [ebx], 0xFFFFFFFF ; *p ^= 0xFFFFFFFF;
    cmp     esi, [ebx]              ; if(*p != pat0)
    jne     mts_fin
    mov     [ebx], edx              ; *p = old;
    add     eax, 0x1000             ; i += 0x1000;
    cmp     eax, [esp+12+8]         ; if(i <= end)
    jbe     mts_loop
    pop     ebx
    pop     esi
    pop     edi
    ret
mts_fin:
    mov     [ebx], edx              ; *p = old
    pop     ebx
    pop     esi
    pop     edi
    ret

task_switch_4:
    jmp     4*8:0
    ret

task_switch_3:
    jmp     3*8:0
    ret
    