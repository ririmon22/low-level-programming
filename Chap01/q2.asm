section .text
global _start
_start:
    ; 1. raxに初期値を設定
    mov rax, 0xAAAAAAAABBBBBBBB

    ; 2.eax(32ビット部分)に書き込み
    mov eax, 0xDEADBEEF

    ; raxの値を終了コードとして返す(rax -> rdi)
    mov rdi, rax
    mov rax, 60 ; exit syscall
    syscall