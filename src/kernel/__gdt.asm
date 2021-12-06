[bits 64]
global gdt_load

; 0x00 : null
; 0x08 : kernel code
; 0x10 : kernel data

gdt_load:
    lgdt [rdi] ; load the GDT given as a function parameter
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi
    mov rax, 0x08
    push rax
    push rdi
    retfq