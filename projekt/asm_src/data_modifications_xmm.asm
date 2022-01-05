
;; Eksport symboli, tak aby linker i C++ mógł zaimportować je do programu
global _apply_data_modifications_asm
global _get_data_chunks_from_pixel_array_asm
global _combine_data_chunks_into_bytes_asm

;; funkcja aplikująca zmiany do tablicy pixeli,
_apply_data_modifications_asm:
        not     edx             ; negacja maski
        pinsrb  xmm1, edx, 0    ; przepisanie maski do rejestru xmm
        pinsrb  xmm1, edx, 8    ; potrzebne do wykonania obliczeń wektorowo
        pinsrb  xmm1, edx, 16
        pinsrb  xmm1, edx, 24
        pinsrb  xmm1, edx, 32
        pinsrb  xmm1, edx, 40
        pinsrb  xmm1, edx, 48
        pinsrb  xmm1, edx, 56
        movsx   r8, ecx         ; załadowanie wielkości tablicy do rejestru r8
        sub     r8, 7           ; odjęcie od wielkości tablicy 7, by nie wyjść poza przydzieloną pamięć
        mov     rax, 0          ; wyzerowanie akumulatora
        cmp     rcx, 0          ; jeżeli długość tablicy to 0, nie wykonuj nic
        je      EXIT
LOOP_MAIN:                      ; główna pętla funkcji
        movzx   ecx, BYTE [rdi+rax]
        movdqu  xmm0, [rdi+rax]     ; załadowanie do rejestru xmm0 8 bajtów z tablicy pixeli
        movdqu  xmm2, [rsi+rax]     ; załadowanie do rejestru xmm2 8 bajtów z tablicy danych do ukrycia
        pand    xmm0, xmm1          ; aplikacja maski
        por     xmm0, xmm2          ; podmiana ostatnich bitów na poprawne wartości
        movups  [rdi+rax], xmm0     ; zapis zmodyfikowanych pixeli do tablicy
        cmp     rax, 1              ; test czy to pierwsza iteracja, gdzie pojawiał się błąd
        je      FIRST_BYTE_MITIGATION
CONTINUE:
        add     rax, 8          ; inkrementacja licznika pętli, o 8 wartości bo tyle jest ładowanych do xmm0
        cmp     r8, rax         ; test czy większe od wielkości tablicy
        jg     LOOP_MAIN        ; skok jeżeli nie
EXIT:
        ret

FIRST_BYTE_MITIGATION:          ; analogicznie do powyższych instrukcji, tylko dla drugiego bajtu, który się uszkadzał
        movzx   ecx, BYTE [rdi + 1]
        and     cl, dl
        or      cl, BYTE [rsi + 1]
        mov     BYTE [rdi + 1], cl
        jmp     CONTINUE

;;; funkcja wyciągająca podzielone paczki bitów z pixeli
_get_data_chunks_from_pixel_array_asm:
        test    ecx, ecx        ; sprawdzenie, czy długość tablicy to nie 0
        jle     SECOND_EXIT     ; jeżeli 0 zakończ funkcję
        pinsrb  xmm1, edx, 0    ; załadowanie maski do rejestru xmm1
        pinsrb  xmm1, edx, 8
        pinsrb  xmm1, edx, 16
        pinsrb  xmm1, edx, 24
        pinsrb  xmm1, edx, 32
        pinsrb  xmm1, edx, 40
        pinsrb  xmm1, edx, 48
        pinsrb  xmm1, edx, 56
        mov     r8d, ecx        ; załadowanie wielkości tablicy do rejestru r9
        sub     r8d, 8          ; odjęcie 8 od wielkości tablicy, aby nie wyjść poza przydzieloną pamięć
        mov     rax, 0          ; wyzerowanie akumulatora
SECOND_MAIN_LOOP:               ; główna pętla
        movdqu  xmm0, [rdi+rax] ; załadowanie 8 bajtów z tablicy pixeli do rejestru xmm0
        pand    xmm1, xmm0      ; aplikacja maski
        movups  [rsi+rax], xmm0 ; zapis 8 paczek bitów do tablicy wynikowej (przejściowej)
        add     rax, 8          ; inkrementacja licznika o 8 pozycji
        cmp     rax, r8         ; sprawdzenie czy nie przekroczono wielkości tablicy
        jl      SECOND_MAIN_LOOP
SECOND_EXIT:
        ret

;;; funkcja sklejająca paczki bitów w gotowe bajty danych
_combine_data_chunks_into_bytes_asm:
        test    ecx, ecx        ; sprawdzenie czy wielkość tablicy nie jest równa 0
        jle     EXIT_3          ; jeżeli równa 0, zakończ
        lea     eax, [rcx-1]    ; załaduj maskę do akumulatora
        push    rbx             ; chyba niepotrzebne TODO: usuń mnie
        mov     r10, rsi        ; załaduj wielkość
        lea     r9d, [rdx+rdx]  ; załaduj wielkość
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
EXIT_3:
        ret
