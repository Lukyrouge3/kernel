bits 32
section .text

global move_stack_pointer
; void move_stack_pointer(uint32_t new_sp);

move_stack_pointer:
    mov eax, [esp + 4]  ; Get new_sp parameter

    mov edx, [esp]        ; edx = return address (from old stack)

    mov esp, eax        ; Set stack pointer to new value
    push edx              ; put return address on new stack
    ret