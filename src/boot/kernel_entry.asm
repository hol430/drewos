
; Entrypoint to the kernel's main function.
[bits 32]

; main symbol will be resolved by the linker.
[extern main]

; Enter the main function.
call main

; Hang if the main function ever returns.
jmp $
