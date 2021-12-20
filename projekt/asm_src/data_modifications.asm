global _apply_data_modifications_asm

_apply_data_modifications_asm:
        not     edx
        movsx   r8, ecx
        mov     eax, 0
        cmp     ecx, 0
        je      EXIT
LOOP_1:
        movzx   ecx, BYTE [rdi+rax]
        and     ecx, edx
        or      cl, BYTE [rsi+rax]
        mov     BYTE [rdi+rax], cl
        add     rax, 1
        cmp     r8, rax
        jne     LOOP_1
EXIT:
        ret