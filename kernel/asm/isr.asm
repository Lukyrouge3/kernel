bits 32
section .text

; Macro pour créer un ISR sans code d'erreur
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push dword 0          ; Code d'erreur factice
    push dword %1         ; Numéro d'interruption
    jmp isr_common_stub
%endmacro

; Macro pour créer un ISR avec code d'erreur
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push dword %1         ; Numéro d'interruption
    jmp isr_common_stub
%endmacro

IDT_DEF:
    ; Définition des 32 premiers ISR (exceptions CPU)
    ISR_NOERRCODE 0   ; Division par zéro
    ISR_NOERRCODE 1   ; Debug
    ISR_NOERRCODE 2   ; NMI
    ISR_NOERRCODE 3   ; Breakpoint
    ISR_NOERRCODE 4   ; Overflow
    ISR_NOERRCODE 5   ; Bound range exceeded
    ISR_NOERRCODE 6   ; Invalid opcode
    ISR_NOERRCODE 7   ; Device not available
    ISR_ERRCODE   8   ; Double fault
    ISR_NOERRCODE 9   ; Coprocessor segment overrun
    ISR_ERRCODE   10  ; Invalid TSS
    ISR_ERRCODE   11  ; Segment not present
    ISR_ERRCODE   12  ; Stack-segment fault
    ISR_ERRCODE   13  ; General protection fault
    ISR_ERRCODE   14  ; Page fault
    ISR_NOERRCODE 15  ; Réservé
    ISR_NOERRCODE 16  ; x87 FPU error
    ISR_ERRCODE   17  ; Alignment check
    ISR_NOERRCODE 18  ; Machine check
    ISR_NOERRCODE 19  ; SIMD floating-point exception
    ISR_NOERRCODE 20  ; Virtualization exception
    ISR_ERRCODE   21  ; Control protection exception
    ISR_NOERRCODE 22  ; Réservé
    ISR_NOERRCODE 23  ; Réservé
    ISR_NOERRCODE 24  ; Réservé
    ISR_NOERRCODE 25  ; Réservé
    ISR_NOERRCODE 26  ; Réservé
    ISR_NOERRCODE 27  ; Réservé
    ISR_NOERRCODE 28  ; Hypervisor injection exception
    ISR_ERRCODE   29  ; VMM communication exception
    ISR_ERRCODE   30  ; Security exception
    ISR_NOERRCODE 31  ; Réservé

extern isr_handler

isr_common_stub:
    pushad

    mov ax, ds
    push eax    ; Sauvegarde ds

    mov ax, 0x10    ; Charge le segment de données kernel
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popad
    add esp, 8  ; Skip 8 bytes (errno, intno)
    sti
    iret
