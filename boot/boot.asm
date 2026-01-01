bits 16

jmp loader

msg	db	"Hello Mommy!", 0

print:
    lodsb ; MOV character into al, then increments SI
    test al, al ; same as AND al, al but doesnt override first "argument"
    jz done ; jump if ZF == 1
    mov ah,	0x0e
    int 0x10
    jmp print

done:
    ret

loader:
    lgdt [gdt_descriptor]

    mov eax, cr0            ; get into protected mode
    or al, 1
    mov cr0, eax

    jmp 0x08:pm_entry ; For a far jump, we need to tell him which code segment to target, in our case index 1 of our GDT :D


bits 32
pm_entry:
    ; Load data segment registers with data selector
    mov ax, 0x10             ; 0x10 = data selector (3rd descriptor)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000    ; Set stack pointer

    cli ; [CLear InteruptFlag] Clear interupt flags and set to 0
    call 0x00100000
    hlt ; [HaLT] Puts cpu in interupt mode until specific interupts or RESET

gdt_table:
    dq 0x0, 0x0; null
    gdt_code_segment:
        dw 0xffff ; Limit
        dw 0x0 ; Base

        db 0x0
        db 10011010b ; Access byte (Present, DPL=0, Type=1010)
        db 11001111b ; Granularity (Gr=1, Size=1, Limit 16:19=1111)
        db 0x0 ; Base (24:31)

    gdt_data_segment:
        dw 0xffff ; Limit
        dw 0x0 ; Base

        db 0x0
        db 10010010b ; Access byte (Present, DPL=0, Type=0010)
        db 11001111b ; Granularity (Gr=1, Size=1, Limit 16:19=1111)
        db 0x0 ; Base (24:31)
    gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_table - 1 ; limit = size - 1
    dw 23
    dd gdt_table

times 510-($-$$) db 0 ; Fill file to 512 bits with 0
dw 0xaa55 ; Boot sector signature