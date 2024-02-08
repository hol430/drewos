; A boot sector that boots a C kernel in 32-bit protected mode.
[org 0x7c00]

; This is the memory offset to which we will load our kernel.
KERNEL_OFFSET equ 0x1000

; BIOS stores our boot drive in dl.
mov [BOOT_DRIVE], dl

; Setup the stack.
mov bp, 0x9000
mov sp, bp

; Print welcome message.
call clrscr
mov bx, MSG_REAL_MODE
call println

; Load the kernel.
call load_kernel

; Switch to protected mode.
call switch_to_pm

jmp $

; Dependencies
%include "util.asm"
%include "gdt.asm"
%include "protected_mode.asm"
%include "util_pm.asm"

[bits 16]

load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call println

    mov bx, KERNEL_OFFSET

    ; Number of sectors to read.
    mov dh, 15

    ; The disk to read from.
    mov dl, [BOOT_DRIVE]

    ; Read.
    call disk_load

    ; debug
    ; ; Print out the first loaded word.
    mov dx, [KERNEL_OFFSET]
    call println_hex

    ret

[bits 32]
; This is where we arrive after switching to and initialising protected mode.
BEGIN_PM:
    ; Print a message to verify successful entry into protected mode.
    mov ebx, MSG_PROT_MODE
    call print_pm

    ; Now jump to the address of our loaded kernel code.
    call KERNEL_OFFSET

    ; Hang.
    jmp $

; Global variables.
BOOT_DRIVE db 0
MSG_REAL_MODE db "Started", 0
MSG_PROT_MODE db "Protected", 0
MSG_LOAD_KERNEL db "Loading kernel", 0

times 510-($-$$) db 0
dw 0xaa55
