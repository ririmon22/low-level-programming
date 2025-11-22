section .data
codes:
    db '0123456789ABCDEF'

section .text
global _start
_start:
    ; この1122...という数字は16進表記
    mov rax, 0x1122334455667788

    mov rdi, 1
    mov rdx, 1
    mov rcx, 64
    ; 4ビットを16進の1桁として出力していくために、
    ; シフトと論理和(AND)によって1桁のデータを得る。
    ; その結果は'codes'配列へのオフセットである。

.loop:
    push rax
    sub rcx, 4
    ; clはレジスタ (rcxの最下位バイト)
    ; rax > eax > ax = (ah + al)
    ; rcx > ecx > cx = (ch + cl)
    sar rax, cl
    and rax, 0xf

    lea rsi, [codes + rax]
    mov rax, 1
    
    ; syscallでrcxとr11が変更される
    push rcx
    syscall
    pop rcx

    pop rax
    ; testは最速の'ゼロか?'チェックに使える
    ; マニュアルで'test'コマンドを参照
    test rcx, rcx
    jnz .loop

    mov rax, 60 ; 'exit'システムコール
    xor rdi, rdi
    syscall