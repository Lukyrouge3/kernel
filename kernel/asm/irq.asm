bits 32
section .text

; Macro pour créer un IRQ sans code d'erreur
%macro IRQ 1
global irq%1
irq%1:
    cli
    push dword 0          ; Code d'erreur factice
    push dword %1         ; Numéro d'interruption
    jmp irq_common_stub
%endmacro

IRQ_DEF:
    ; Définition des 16 IRQs
    IRQ 0   ; Horloge système
    IRQ 1   ; Clavier
    IRQ 2   ; Cascade pour les PICs esclaves
    IRQ 3   ; Port série COM2
    IRQ 4   ; Port série COM1
    IRQ 5   ; Port parallèle LPT2
    IRQ 6   ; Contrôleur de disquette
    IRQ 7   ; Port parallèle LPT1
    IRQ 8   ; Horloge temps réel CMOS
    IRQ 9   ; Libre pour utilisation
    IRQ 10  ; Libre pour utilisation
    IRQ 11  ; Libre pour utilisation
    IRQ 12  ; Souris PS/2
    IRQ 13  ; Coprocesseur mathématique
    IRQ 14  ; Disque dur primaire
    IRQ 15  ; Disque dur secondaire

extern irq_handler

irq_common_stub:
    pushad

    mov ax, ds
    push eax    ; Sauvegarde ds

    mov ax, 0x10    ; Charge le segment de données kernel
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popad
    add esp, 8  ; Skip 8 bytes (error code + intno)
    sti
    iret
