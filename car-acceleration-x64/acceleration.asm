default rel

section .rdata align=4
    kilometers_per_hour_to_meters_per_second: dd 0.277777791043f

section .text
    global acceleration_asm

; Windows x64 ABI:
;   RCX = const float *matrix
;   EDX = int rows
;   R8  = int *output
;
; Each input row is [Vi, Vf, T], all single-precision floats.
; The calculation is:
;   ((Vf - Vi) * 1000 / 3600) / T
;
acceleration_asm:
    test edx, edx
    jle .done

.row_loop:
    movss xmm0, [rcx]                         ; Vi
    movss xmm1, [rcx + 4]                     ; Vf
    subss xmm1, xmm0                          ; Vf - Vi
    mulss xmm1, [kilometers_per_hour_to_meters_per_second]
    movss xmm0, [rcx + 8]                     ; T
    divss xmm1, xmm0
    cvtss2si eax, xmm1                        ; scalar SIMD float to int
    mov [r8], eax

    add rcx, 12
    add r8, 4
    dec edx
    jnz .row_loop

.done:
    ret
