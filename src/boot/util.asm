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

print_lf:
	pusha

	mov ah, 0x0e
	mov al, 0x0d ; CR
	int 0x10
	mov al, 0x0a ; LF
	int 0x10

	popa
	ret

println:
	pusha

	call print
	call print_lf

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

	; use cx as counter; init to 3. Iterate decrementing cx until it reaches 0.
	mov cx, 0x03

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

		; This should never happen - it means the digit was <0 or >15
		.unknown_digit:
			mov bx, ERR_UNKNOWN
			call println

		; Execution should never reach here. This could be safely removed.
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

			; left-bitshift bl 4 times to bring next nibble to MSD
			shl dx, 0x04

			; finish loop if cx == 0 (ie 4th iteration).
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
	call print_lf
	ret

ERR_UNKNOWN:
	db 'Unknown digit', 0

; Read from disk.
; Inputs:
; dh: Number of sectors to read
; dl: Disk from which to read.
disk_load:
	; Store DX on the stack so later we can recall how many sectors were
	; requested to be read, even if it is altered in this function.
	push dx

	; BIOS read sector function.
	mov ah, 0x02

	; Number of sectors to read.
	mov al, dh

	; Select cylinder 0.
	mov ch, 0x00

	; Select head 0.
	mov dh, 0x00

	; Start reading from the second sector (ie after the boot sector).
	mov cl, 0x02

	; Raise the BIOS interrupt to execute the read.
	int 0x13

	; Jump iff the carry flag was set.
	jc disk_error

	disk_load_success:
		; Restore dx from the stack.
		pop dx
		ret

	disk_nsect_error:
		mov bx, DISK_ERROR_MSG
		call print
		mov bx, DISK_ERROR_MSG_CTX
		call print
		mov bx, DISK_ERROR_MSG_NSECT
		call println
		jmp $

	disk_error:
		; The number of sectors we actually read is stored in al.
		; TODO: if this doesn't match the expected number of sectors, should we
		; error out? For now, I'll treat this as success, otherwise when loading
		; the kernel we would need to count the number of sectors occupied by
		; the kernel every time we recompile it.

		; Check if we read the correct number of sectors.
		cmp al, dh
		jne disk_load_success
		; jne disk_nsect_error

		mov bx, DISK_ERROR_MSG
		call print
		mov bx, DISK_ERROR_MSG_CTX
		call print
		mov dh, ah
		mov dl, 0x00
		call println_hex
		jmp $

; Global variables
DISK_ERROR_MSG: db "Disk read error", 0
DISK_ERROR_MSG_CTX: db ": ", 0
DISK_ERROR_MSG_NSECT: db "Failed to read specified number of sectors", 0
DISK_ERROR_MSG_GENERIC: db "i/o error", 0

; Error codes:
; 0x00	successful completion
; 0x01	invalid function in AH or invalid parameter
; 0x02	address mark not found
; 0x03	disk write-protected
; 0x04	sector not found/read error
; 0x05	reset failed (hard disk)
; 0x05	data did not verify correctly (TI Professional PC)
; 0x06	disk changed (floppy)
; 0x07	drive parameter activity failed (hard disk)
; 0x08	DMA overrun
; 0x09	data boundary error (attempted DMA across 64K boundary or >80h sectors)
; 0x0A	bad sector detected (hard disk)
; 0x0B	bad track detected (hard disk)
; 0x0C	unsupported track or invalid media
; 0x0D	invalid number of sectors on format (PS/2 hard disk)
; 0x0E	control data address mark detected (hard disk)
; 0x0F	DMA arbitration level out of range (hard disk)
; 0x10	uncorrectable CRC or ECC error on read
; 0x11	data ECC corrected (hard disk)
; 0x20	controller failure
; 0x31	no media in drive (IBM/MS INT 13 extensions)
; 0x32	incorrect drive type stored in CMOS (Compaq)
; 0x40	seek failed
; 0x80	timeout (not ready)
; 0xAA	drive not ready (hard disk)
; 0xB0	volume not locked in drive (INT 13 extensions)
; 0xB1	volume locked in drive (INT 13 extensions)
; 0xB2	volume not removable (INT 13 extensions)
; 0xB3	volume in use (INT 13 extensions)
; 0xB4	lock count exceeded (INT 13 extensions)
; 0xB5	valid eject request failed (INT 13 extensions)
; 0xB6	volume present but read protected (INT 13 extensions)
; 0xBB	undefined error (hard disk)
; 0xCC	write fault (hard disk)
; 0xE0	status register error (hard disk)
; 0xFF	sense operation failed (hard disk)
