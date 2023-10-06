print_string:
    mov ah, 0x0E         ; AH = 0x0E for teletype mode
.loop:
    mov al, [ds:dx]      ; Load the next character from the string
    cmp al, 0            ; Check if it's the null terminator
    je .done             ; If it is, we're done
    int 0x10             ; Otherwise, print the character
    inc dx               ; Move to the next character
    jmp .loop            ; Repeat the process for the next character
.done:
    ret

section .text
    global _start
_start:
    ; Assuming that dx points to your null-terminated string
    mov dx, your_string  ; Load the address of your string into dx
    call print_string    ; Call the print_string function

    ; Exit program (optional)
    int 0x10             ; Call BIOS function (e.g., to return to bootloader)
    xor ah, ah           ; AH = 0 (indicates successful termination)
    int 0x16             ; Wait for a key press
    int 0x19             ; Reboot

section .data
    your_string db 'Hello, world!', 0