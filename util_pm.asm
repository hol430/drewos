
; Define some constants.

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f
GREEN_ON_BLACK equ 0x03

print_pm:
    pusha

    ; Set edx to the start of video memory.
    mov edx, VIDEO_MEMORY
    add edx, 640

    ; Store the attributes in AH
    mov ah, GREEN_ON_BLACK

    print_pm_loop:
        ; Store the character at ebx in al
        mov al, [ebx]

        ; if al == 0, we are at end of string.
        cmp al, 0
        je print_pm_done

        ; Store character and attributes at current character cell.
        ; mov al, 'X'
        mov [edx], ax

        ; Increment ebx to the next character in the string.
        add ebx, 1

        ; Move to the next character cell in vieo memory.
        add edx, 2

        ; Iterate around to print the next character.
        jmp print_pm_loop

    print_pm_done:
        popa
        ret
