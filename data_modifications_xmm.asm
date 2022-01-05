global _apply_data_modifications_asm
global _get_data_chunks_from_pixel_array_asm
global _combine_data_chunks_into_bytes_asm

_apply_data_modifications_asm:
        not     edx
        pinsrb  xmm1, edx, 0
        pinsrb  xmm1, edx, 8
        pinsrb  xmm1, edx, 16
        pinsrb  xmm1, edx, 24
        pinsrb  xmm1, edx, 32
        pinsrb  xmm1, edx, 40
        pinsrb  xmm1, edx, 48
        pinsrb  xmm1, edx, 56
        movsx   r8, ecx
        sub     r8, 7
        mov     rax, 0
        cmp     rcx, 0
        je      EXIT
LOOP_MAIN:
        movzx   ecx, BYTE [rdi+rax]
        movdqu  xmm0, [rdi+rax]
        movdqu  xmm2, [rsi+rax]
        pand    xmm0, xmm1
        por     xmm0, xmm2
        movups  [rdi+rax], xmm0
        cmp     rax, 1
        je      FIRST_BYTE_MITIGATION
CONTINUE:
        add     rax, 8
        cmp     r8, rax
        jg     LOOP_MAIN
EXIT:
        ret

FIRST_BYTE_MITIGATION:
        movzx   ecx, BYTE [rdi + 1]
        and     cl, dl
        or      cl, BYTE [rsi + 1]
        mov     BYTE [rdi + 1], cl
        jmp     CONTINUE


_get_data_chunks_from_pixel_array_asm:
        cmp     ecx, 0
        je     EXIT
        pinsrb  xmm1, edx, 0
        pinsrb  xmm1, edx, 8
        pinsrb  xmm1, edx, 16
        pinsrb  xmm1, edx, 24
        pinsrb  xmm1, edx, 32
        pinsrb  xmm1, edx, 40
        pinsrb  xmm1, edx, 48
        pinsrb  xmm1, edx, 56
        movsx   r8, ecx
        sub     r8, 15
        mov     rax, 0

LOOP_SECOND_FUN:
        movzx   ecx, BYTE [rdi+rax]
        movdqu  xmm0, [rdi+rax]
        pand    xmm0, xmm1
        movups  [rsi+rax], xmm0
        add     rax, 8
        cmp     r8, rax
        jg      LOOP_SECOND_FUN
        jmp     EXIT

;; _get_data_chunks_from_pixel_array_asm:
;;         test    ecx, ecx
;;         jle     .L1
;;         mov     r8d, ecx
;;         mov     eax, 0
;; .L3:
;;         mov     ecx, edx
;;         and     cl, BYTE [rdi+rax]
;;         mov     BYTE [rsi+rax], cl
;;         add     rax, 1
;;         cmp     rax, r8
;;         jne     .L3
;; .L1:
;;         ret

_combine_data_chunks_into_bytes_asm:
        cmp     ecx, 0
        jle     EXIT
        lea     eax, [rcx-1]
        push    rbx
        mov     r10, rsi
        lea     r9d, [rdx+rdx]
        lea     rbx, [rsi+1+rax]
        mov     r11d, 0
F3_OUTER_LOOP:
        mov     r8d, 0
        test    edx, edx
        jle     F3_OUTER_SAVE
        movsx   rsi, r11d
        mov     ecx, 0
        mov     r8d, 0
        add     rsi, rdi
F3_INNER_LOOP:
        movzx   eax, BYTE [rsi]
        add     rsi, 1
        sal     eax, cl
        add     ecx, 2
        add     r8d, eax
        cmp     r9d, ecx
        jne     F3_INNER_LOOP
F3_OUTER_SAVE:
        mov     BYTE [r10], r8b
        add     r10, 1
        add     r11d, edx
        cmp     rbx, r10
        jne     F3_OUTER_LOOP
        pop     rbx
        ret
