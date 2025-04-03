%macro isr_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

global generic_isr_stub
generic_isr_stub:
    pusha                   ; Save all general-purpose registers
    mov eax, [esp + 32]     ; Retrieve interrupt vector from the stack
    push eax                ; Push vector number onto the stack
    call generic_handler    ; Call a single C handler
    add esp, 4              ; Clean up pushed vector
    popa                    ; Restore all registers
    mov al, 0x20            ; Send end-of-interrupt (EOI) to PIC
    out 0x20, al
    iret                    ; Return from the interrupt

extern exception_handler
extern generic_handler

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i+1
%endrep
