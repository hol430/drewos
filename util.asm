print:
	; input is in bx
	pusha
	mov ah, 0x0e

	.print_start:
		mov al, [bx]
		cmp al, 0

		; if null terminator is found, we're done
		je .print_end

		; else we can print the current character
		int 0x10

		; Move to next byte.
		inc bx

		jmp .print_start

	.print_end:
		popa
		ret

println:
	pusha

	call print

	mov ah, 0x0e
	mov al, 0x0d ; CR
	int 0x10
	mov al, 0x0a ; LF
	int 0x10

	popa
	ret

clrscr:
	pusha

	mov ah, 0x06 ; scroll up function
	mov al, 0x00 ; clear entire screen
	mov bh, 0x07 ; attribute, assuming white on black
	mov cx, 0x00 ; start row/column
	mov dh, 24   ; end row
	mov dl, 79   ; end column
	int 0x10

	mov ah, 0x02 ; set cursor position function
	mov al, 0x00 ; unused?
	mov bh, 0x00 ; 0-3 in modes 2&3, 0-7 in modes 0&1, 0 in graphics modes
	mov bl, 0x00 ; unused?
	mov dh, 0x00 ; row: 0 is top
	mov dl, 0x00 ; column: 0 is left
	int 0x10

	popa
	ret
