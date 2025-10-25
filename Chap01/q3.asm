section .text
global _start
_start:
    mov rax, 10
    mov rbx, 20

    cmp rax, rbx ; ZF=0 (Not Zero)になる

    jnz .different

    ; ここには到達しない
    mov rdi, 1 ; 失敗コード
    jmp .exit

.different:
    ; 成功時の処理
    mov rdi, 0 ; 成功コード

.exit:
    mov rax, 60
    syscall