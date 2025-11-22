; jamp_example.asm

section .text
global _start

_start:
    ; raxに比較したい値をセット
    mov rax, 10 ; 比較したい数値

    cmp rax, 42 ; rax < 42か？
    jl less     ; 小さいならlessへジャンプ

    mov rbx, 0  ; raxが42以上ならrbxを0に
    jmp end

less:
    mov rbx, 1  

end:
    ; exit(rbx)
    mov rax, 60
    mov rdi, rdx
    syscall