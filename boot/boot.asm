org 0x7c00
    bits 16

jmp loader

msg	db	"Hello Mommy!", 0

print:
    lodsb
    test al, al
    jz done
    mov ah,	0x0e
    int 10h
    jmp print

done:
    ret

loader:
    mov si, msg
    call print
    cli
    hlt

times 510-($-$$) db 0
dw 0xaa55 ; Boot sector signature