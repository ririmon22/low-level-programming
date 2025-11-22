global _start

section .data
message:db 'hello, world!', 10

section .text
_start:
    mov rax, 1 ; システムコールの番号をraxに入れる
    mov rdi, 1 ; 引数 #1はrdi: 書き込み先 (descriptor)
    mov rsi, message ; 引数 #2はrsi: 文字列の先頭
    mov rdx, 14 ; 引数#3はrdx: 書き込むバイト数
    syscall ; この命令がシステムコールを呼び出す
    
    mov rax, 60 ; 'exit'のsyscall番号
    xor rdi, rdi
    syscall
