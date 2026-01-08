bits 16
org 0x7c00

jmp loader

MEMORY_MAP_ADDR equ 0x8000
KERNEL_SECTORS_ADDR equ 0x7FFC

%define KERNEL_SRC_SEG  0x1000   ; 0x1000:0x0000 = 0x00010000
%define KERNEL_SRC_OFF  0x0000

%define KERNEL_DST_SEG  0xFFFF   ; 0xFFFF:0x0010 = 0x00100000
%define KERNEL_DST_OFF  0x0010

copy_kernel_to_1mib:
    push ds
    push es
    push si
    push di
    push cx
    push ax

    ; DS:SI = source
    mov ax, KERNEL_SRC_SEG
    mov ds, ax
    mov si, KERNEL_SRC_OFF

    ; ES:DI = destination (1 MiB)
    mov ax, KERNEL_DST_SEG
    mov es, ax
    mov di, KERNEL_DST_OFF

    ; Copy words
    mov cx, (KERNEL_SECTORS * 512) / 2
    cld
    rep movsw

    ; If odd byte count, copy last byte
    %if (KERNEL_SECTORS * 512) % 2
        movsb
    %endif

    pop ax
    pop cx
    pop di
    pop si
    pop es
    pop ds
    ret

done:
    ret

detect_memory:
    mov di, MEMORY_MAP_ADDR
    xor ebx, ebx
    xor bp, bp
    mov edx, 0x534D4150
    
.e820_loop:
    mov eax, 0xE820
    mov ecx, 24               ; Request 24 bytes
    int 0x15
    
    jc .e820_failed
    cmp eax, 0x534D4150
    jne .e820_failed
    
    ; Check how many bytes were actually returned in ECX
    cmp ecx, 20
    jl .e820_failed           ; Less than 20 bytes = invalid
    
    ; Move to next entry based on actual size returned
    add di, cx                ; Use CX (actual bytes) not hardcoded 24!
    inc bp
    
    test ebx, ebx
    je .e820_done
    
    cmp bp, 100
    jge .e820_done
    
    jmp .e820_loop

.e820_failed:
    cmp bp, 0                 ; Did we get at least one entry?
    jne .e820_done            ; If yes, continue anyway

.e820_done:
    ; Store entry count at the beginning (before the first entry)
    mov [MEMORY_MAP_ADDR - 2], bp
    ret

loader:
    call detect_memory

    ; Store kernel sectors at a known location for kernel to read
%ifndef KERNEL_SECTORS
    %define KERNEL_SECTORS 20
%endif
    mov word [KERNEL_SECTORS_ADDR], KERNEL_SECTORS

    ; Enable A20 line
    mov ax, 0x2401      ; Enable A20 via BIOS
    int 0x15

    ; Load kernel from disk (sector 2 onwards) to 0x1000:0x0000 (0x10000)
    mov ah, 0x02        ; Read sectors function
    mov al, KERNEL_SECTORS  ; Number of sectors to read (set at compile time)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2 (sector 1 is index 1, but CHS is 1-indexed)
    mov dh, 0           ; Head 0
    mov dl, 0           ; Drive 0 (floppy)
    mov bx, KERNEL_SRC_SEG      ; Segment
    mov es, bx
    xor bx, bx          ; Offset 0 (so ES:BX = 0x1000:0x0000 = 0x10000)
    int 0x13            ; BIOS disk interrupt
    jc disk_error       ; Jump if carry flag set (error)

    call copy_kernel_to_1mib

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

    mov esp, 0x90000   ; Set stack to 576KB (well above kernel and bitmap)

    cli ; [CLear InterruptFlag] Clear interrupt flags and set to 0
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