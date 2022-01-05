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
        test    ecx, ecx
        jle     SECOND_EXIT
        pinsrb  xmm1, dl, 0
        pinsrb  xmm1, dl, 8
        pinsrb  xmm1, dl, 16
        pinsrb  xmm1, dl, 24
        pinsrb  xmm1, dl, 32
        pinsrb  xmm1, dl, 40
        pinsrb  xmm1, dl, 48
        pinsrb  xmm1, dl, 56
        mov     r8d, ecx
        sub     r8d, 1
        mov     rax, 0
SECOND_MAIN_LOOP:
        movdqu  xmm0, [rdi+rax]
        ;; pand    xmm0, xmm1
        movups  [rsi+rax], xmm0
        add     rax, 8
        cmp     rax, r8
        jl      SECOND_MAIN_LOOP
SECOND_EXIT:
        ret
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
        test    ecx, ecx
        jle     EXIT_3
        lea     eax, [rcx-1]
        push    rbx
        mov     r10, rsi
        lea     r9d, [rdx+rdx]
        lea     rbx, [rsi+1+rax]
        xor     r11d, r11d
FIRST_LOOP_3:
        xor     r8d, r8d
        test    edx, edx
        jle     FINAL_COMPUTE_3
        movsx   rsi, r11d
        xor     ecx, ecx
        xor     r8d, r8d
        add     rsi, rdi
SECOND_LOOP_3:
        movzx   eax, BYTE [rsi]
        add     rsi, 1
        sal     eax, cl
        add     ecx, 2
        add     r8d, eax
        cmp     r9d, ecx
        jne     SECOND_LOOP_3
FINAL_COMPUTE_3:
        mov     BYTE [r10], r8b
        add     r10, 1
        add     r11d, edx
        cmp     r10, rbx
        jne     FIRST_LOOP_3
        pop     rbx
        ret
EXIT_3:
        ret
