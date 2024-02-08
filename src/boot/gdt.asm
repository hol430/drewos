; GDT
gdt_start:

; The mandatory null descriptor.
gdt_null:
    dd 0x0
    dd 0x0

; Code segment descriptor.
gdt_code:
    ; base = 0x0, limit = 0xfffff
    ; first flags: (present)1 (privilege)00 (descriptor type)1 -> 1001b
    ; type flags : (code)1 (conforming)0 (readable)1 (accessed)0 -> 1010b
    ; 2nd flags: (granularity)1 (32-bit default)1 (64-bit seg)0 (AVL)0 -> 1100b

    ; Limit (bits 0-15)
    dw 0xffff

    ; Base (bits 0-15)
    dw 0x0000

    ; Base (bits 16-23)
    db 0x00

    ; First flags and type flags.
    db 10011010b

    ; 2nd flags, limit (bits 16-19)
    db 11001111b

    ; Base (bits 24-31)
    db 0x00

; Data segment descriptor.
gdt_data:
    ; Same as code segment except for type flags.
    ; Type flags: (code)0 (expand down)0 (writable)1 (accessed)0 -> 0010b

    ; Limit (bits 0-15)
    dw 0xffff

    ; Base (bits 0-15)
    dw 0x0000

    ; Base (bits 16-23)
    db 0x00

    ; First flags, type flags
    db 10010010b

    ; Second flags, limit (bits 16-19)
    db 11001111b

    ; Base (bits 24-31)
    db 0x00

; The reason for putting a label at the end of the GDT is so we can have the
; assembler calculate the size of the GDT for the GDT descriptor below.
gdt_end:

; GDT descriptor.
gdt_descriptor:
    ; Size of our GDT, always less one of the true size.
    dw gdt_end - gdt_start - 1

    ; Start address of our GDT.
    dd gdt_start

; Define some useful constants for the GDT segment descriptor offsets, which are
; what segment registers must contain hwne in protected mode. For example, when
; we set DS = 0x10 in protected mode, the CPU knows that we mean it to use the
; segment described at offset 0x10 (ie 16 bytes) in our GDT, which in our case
; is the data segment (0x0 -> NULL; 0x08 -> CODE; 0x10 -> DATA).

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
