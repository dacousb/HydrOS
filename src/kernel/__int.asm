[bits 64]

; keep in mind that
; cli:  clears the interrupt flag
; iret: returns from an interrupt

extern isr_handler
extern irq_handler
global interrupt

; with the IDT we will tell the CPU that we want it to jump
; to the isr0 (and so on) label when interrupt 0 is fired

; the isr0 label (isr%1, where %1 = 0) will push the interrupt
; id (push %1, where variable %1 = 0) to the stack and call the handler

; pushing to the stack is equivalent to passing function parameters
; see isr.c, the isr handler accepts an id parameter (same for IRQs)

%macro def_isr 1
    global isr%1
    isr%1:
        cli
        push 0
        push %1
        jmp isr_stub
%endmacro

%macro def_irq 1
    global irq%1
    irq%1:
        cli
        push 0
        push (%1 + 32)
        jmp irq_stub
%endmacro

%macro push_regs 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rsi, rsp

    mov rdi, [rsp + 120]
    mov rsi, rsp
%endmacro

%macro pop_regs 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16
%endmacro

isr_stub:
    push_regs 
    call isr_handler
    pop_regs

    iretq

irq_stub:
    push_regs
    call irq_handler
    pop_regs

    iretq

def_isr 0
def_isr 1
def_isr 2
def_isr 3
def_isr 4
def_isr 5
def_isr 6
def_isr 7
def_isr 8
def_isr 9
def_isr 10
def_isr 11
def_isr 12
def_isr 13
def_isr 14
def_isr 15
def_isr 16
def_isr 17
def_isr 18
def_isr 19
def_isr 20
def_isr 21
def_isr 22
def_isr 23
def_isr 24
def_isr 25
def_isr 26
def_isr 27
def_isr 28
def_isr 29
def_isr 30
def_isr 31

def_irq 0 ; 32. system timer
def_irq 1 ; 33. keyboard
def_irq 2 ; 34. PIC cascading (never raised)