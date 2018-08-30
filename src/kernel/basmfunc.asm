GLOBAL io_load_eflags, io_store_eflags
GLOBAL load_cr0, store_cr0

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
