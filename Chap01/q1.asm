section .text
global _start
_start:
    ; 1. 0x1234をプッシュ
    push qword 0x1234
    ; 2. 0x5678をプッシュ
    push qword 0x5678

    ; 3. スタックから値を復元（LIFO順）
    pop rax
    pop rbx
    
    mov rax, 60
    xor rdi, rdi
    sysca