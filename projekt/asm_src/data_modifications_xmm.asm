global _apply_data_modifications_asm

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

