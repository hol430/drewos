[bits 16]
; Switch to protected mode.
switch_to_pm:
    ; We must disable interrupts until we have set up the protected mode
    ; interrupt vector, otherwise interrupts will run riot.
    cli

    lgdt [gdt_descriptor]

    ; To make the switch to protected mode, we set the first bit of CR0, a
    ; register.
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:init_pm

[bits 32]

; Initialise registers and the stack once in protected mode.
init_pm:
    mov ax, DATA_SEG

    ; Now that we are in protected mode, our old segments are meaningless, so we
    ; point our segment registers to the data segment we defined in our GDT.
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Update our stack position so it is right at the top of the free space.
    mov ebp, 0x90000
    mov esp, ebp

    ; Finally, call some well-known label.
    call BEGIN_PM
