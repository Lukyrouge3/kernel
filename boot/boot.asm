org 0x7c00 ; [ORiGin] tells our code where it's origin is. bios always loads bootloaders at this address
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
    mov si, msg
    call print
    cli ; [CLear InteruptFlag] Clear interupt flags and set to 0
    hlt ; [HaLT] Puts cpu in interupt mode until specific interupts or RESET

times 510-($-$$) db 0 ; Fill file to 512 bits with 0
dw 0xaa55 ; Boot sector signature