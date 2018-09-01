GLOBAL io_load_eflags, io_store_eflags
GLOBAL load_cr0, store_cr0
GLOBAL memtest_asm

[SECTION .text]

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
    