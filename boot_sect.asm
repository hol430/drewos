[org 0x7c00]

; Main program.
call clrscr

; Print welcome message.
mov bx, HELLO_MSG
call println

; Print end message.
mov bx, GOODBYE_MSG
call println

jmp .hang

; data
HELLO_MSG:
	; Remember the NULL terminator!!
	db 'Welcome to drewOS.', 0
GOODBYE_MSG:
	db 'Thanks for using drewOS!', 0

%include "util.asm"

.hang:

; Jump to the current address (ie forever)
jmp $

; Pad to 510 bytes
times 510-($-$$) db 0

; Write the magic number which indicates to BIOS that this is a boot sector.
dw 0xaa55
