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

; Print the value stored in dx in hexadecimal.
print_hex:
	pusha

	; use cx as counter; init to 0
	xor cx, 0x03

	; init teletype mode
	mov ah, 0x0e

	; print '0x'
	mov al, '0'
	int 0x10
	mov al, 'x'
	int 0x10

	.print_hex_loop_start:
		mov bx, dx

		; right-bitshift by 12
		shr bx, 0x0c

		; rightmost nibble of bx now contains the c-th most significant
		; nibble of dx
		cmp bx, 0xf
		jg .unknown_digit
		cmp bx, 0
		jl .unknown_digit

		cmp bl, 9
		jg .letter
		jle .number

		; This should never happen
		.unknown_digit:
			mov bx, ERR_UNKNOWN
			call println

		jmp .print_hex_end

		.number:
			add bl, '0'
			jmp .print_hex_loop_end

		.letter:
			add bl, 0x57 ; 0x57 = 87d, 'a' is 97d
			jmp .print_hex_loop_end

		.print_hex_loop_end:
			mov al, bl
			int 0x10

			; left-bitshift bl c times to bring next nibble to MSD
			shl dx, 0x04

			; finish loop if counter ==0
			cmp cx, 0
			je .print_hex_end

			; decrement counter
			dec cx

			jmp .print_hex_loop_start

	.print_hex_end:
		popa
		ret

println_hex:
	call print_hex
	mov ah, 0x0e
	mov al, 0x0d ; CR
	int 0x10
	mov al, 0x0a ; LF
	int 0x10
	ret

ERR_UNKNOWN:
	db 'Unknown digit', 0
