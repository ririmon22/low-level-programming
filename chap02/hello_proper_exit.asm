global _start
section .data
message: db 'hello, world', 10

section .text
global _start

_start:
    mov rax, 1     ;'write'syscall番号
    mov rdi, 1     ;stdoutのディスクリプタ
    mov rsi, message ; 文字列のアドレス
    mov rdx, 14     ;文字列のバイト数
    syscall

    mov rax, 60    ;'exit'のsyscall番号
    xor rdi, rdi
    syscall