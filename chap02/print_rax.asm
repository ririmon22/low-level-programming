section .data
codes:
    db '0123456789ABCDEF'

section .text
global _start
_start:
    ; store number in general-purpose registers
    mov rax, 0x1122334455667788 ; output number
    mov rdi, 1 ; write() first argument (stdout)
    mov rdx, 1 ; write() third argument (output length)
    mov rcx, 64 ; 64bit counter

.loop:
    push rax  ; raxの値を取得
    sub rcx, 4 ; 4bitずつ処理するのでカウントを減らす
    sar rax, cl ; raxをclビット右シフト
    and rax, 0xf ; 下位4ビットだけを取り出す。

    lea rsi, [codes + rax]; その値をcodes テーブルのオフセットに
    mov rax, 1 ; write syscall
    push rcx ; syscallでrcxが上書きされるため退避
    syscall ; write(1, rsi, 1)
    pop rcx ; rcxを戻す
    pop rax ; raxを戻す
    test rcx, rcx ; rcxが0かチェック
    jnz .loop ; 0でなければループ

mov rax, 60
xor rdi, rdi
syscall
