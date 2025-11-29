section .text
global _start
print_uint:
    sub rsp, 32    ; 32バイトの作業バッファを確保
    mov rbx, rsp   ; rbx = バッファの先頭

    mov rax, rdi   ; 計算対象をRAXに移動(div 用)
    mov rcx, 10    ; 除算の除数 = 10

; --- 1桁ずつ取り出し ---
.loop:
    xor rdx, rdx   ; div用に上位ビットクリア(RDX:RAXを作る)
    div rcx        ; 余り=RDX, 商=RAX

    add dl, '0'    ; 余りをASCIIに変換(余り0なら'0'に)
    mov [rbx], dl  ; バッファに格納
    inc rbx        ; 書き込み位置をずらす

    test rax, rax  ; 商が0なら終了
    jnz .loop

    dec rbx        ; rbxは末尾の次を指しているので戻す
    
; --- 出力処理 ---
    mov rsi, rbx   ; rsi = バッファ末尾
    mov rdi, rbx   ; この値は後で使用
    sub rdi, rsp   ; 末尾 - 先頭 = 書き込んだバイト数 - 1
    inc rdi        ; 実際のバイト数

    mov rax, 1
    mov rdx, rdi
    mov rsi, rsp
    mov rdi, 1
    syscall
    
    add rsp, 32
    ret


parse_uint:
    xor rcx, rcx
    xor rbx, rbx
.jadge_num:
    ; al(raxの最下位バイト)にrdiの中身が入る
    mov al, [rdi + rcx]

    cmp al, '0'
    jl .done

    cmp al, '9'
    jg .done
    
    sub al, '0'

    imul rbx, rbx, 10
    movzx rax, al
    add rbx, rax

    inc rcx
    jmp .jadge_num

.done:
    mov rax, rbx
    mov rdx, rcx
    ret

_start:
    mov rdi, input

    call parse_uint
    mov rdi, rax
    call print_uint

    mov rax, 60
    xor rdi, rdi
    syscall

section .data
    input: db "1234abc", 0