.section ".text"

.extern __libc_deinit
.extern __libc_init
.extern exit
.extern main

.global _start
_start:
    bl      __libc_init
    ldr     x0, [sp], #16
    ldr     x0, [sp], #8
    mov     x1, sp
    bl      main
    bl      __libc_deinit
    bl      exit
