bits 16
org 0x7c00

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
    mov si, msg
    call print

    ; Load kernel from disk (sector 2 onwards) to 0x1000:0x0000 (0x10000)
    mov ah, 0x02        ; Read sectors function
    mov al, 20          ; Number of sectors to read (adjust based on kernel size)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2 (sector 1 is index 1, but CHS is 1-indexed)
    mov dh, 0           ; Head 0
    mov dl, 0           ; Drive 0 (floppy)
    mov bx, 0x1000      ; Segment
    mov es, bx
    xor bx, bx          ; Offset 0 (so ES:BX = 0x1000:0x0000 = 0x10000)
    int 0x13            ; BIOS disk interrupt
    jc disk_error       ; Jump if carry flag set (error)

    ; Enable A20 line
    mov ax, 0x2401      ; Enable A20 via BIOS
    int 0x15

    lgdt [gdt_descriptor]

    mov eax, cr0            ; get into protected mode
    or al, 1
    mov cr0, eax

    jmp dword 0x8:pm_entry ; For a far jump, we need to tell him which code segment to target, in our case index 1 of our GDT :D

disk_error:
    hlt

bits 32
pm_entry:
    ; Load data segment registers with data selector
    mov ax, 0x10             ; 0x10 = data selector (3rd descriptor)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x7c00    ; Set stack pointer

    cli ; [CLear InteruptFlag] Clear interupt flags and set to 0
    jmp 0x10000

gdt_table:
    dq 0x0          ; null descriptor (8 bytes only)
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
    dw gdt_end - gdt_table - 1
    dd gdt_table

times 510-($-$$) db 0 ; Fill file to 512 bits with 0
dw 0xaa55 ; Boot sector signature