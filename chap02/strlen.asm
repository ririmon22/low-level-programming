global _start

section .data

test_string: db "abcdef", 0

section .text

strlen:      ; この関数は、ただ1個の引数をrdiから受け取る。
             ; (われわれの規約による)
    xor rax, rax ; raxの文字列の長さが入る。最初にゼロで初期化
             ; しなければランダムな値になってしまう
.loop:       ; ここからメインループが始まる
    cmp byte [rdi+rax], 0  ; 現在の文字 / 記号が終結のヌルかどうかを調べる。
                           ; ここで'byte'修飾が絶対に必要(cmpのオペランドは
                           ; 左右必ず同じサイズ)
                           ; 右側のオペランドがイミディエートでサイズの情報が
                           ; ないので、メモリから何バイト取り出してゼロと比較
                           ; すればよいのか、'byte'がなければ不明である。
    je .end                ; ヌルを見つけたらジャンプする
    inc rax                ; そうでなければ次の文字へ(カウントアップ) 
    jmp .loop
.end:
    ret                    ; 'ret'に到達したとき、raxに戻り値が入っている
    
_start:
    mov rdi, test_string
    call strlen
    mov rdi, rax
    mov rax, 60
    syscall