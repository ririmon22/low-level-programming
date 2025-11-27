section .data
newline: db 10
minus_char: db '-', 0

section .text

exit:
    ; rdiで受け取った終了コードはそのままsyscallの第一引数に
    ; 流用可能なためexitシステムコールを呼び出すことで対応
    mov rax, 60
    syscall

string_length:
    ; 文字列へのポインタを受け取り、その長さを返す。
    xor rax, rax    ; raxに文字列の長さを入れるために0で初期化する
.loop:
    cmp byte [rdi + rax], 0
    je .end
    inc rax
    jmp .loop
.end:
    ret

print_string:
    ; 文字列へのポインタを受け取り、それを出力する。
    push rdi           ; rdiの中身が書き換わらないように退避する
    call string_length ; 文字列のバイト数を取得する
    pop rdi
    mov rdx, rax       ; 第３引数に文字列のバイト数を格納する
    mov rax, 1         ; 'write'用のシステムコール番号を入れる
    mov rsi, rdi       ; 第２引数に文字列へのポインタを格納
    mov rdi, 1         ; 第１引数に1を指定して書き込み先をstdoutに指定

    syscall
    ret

print_char:
    push rdi

    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall
    
    add rsp, 8
    ret

print_newline:
    mov rax, 1
    mov rdi, 1
    mov rsi, newline
    mov rdx, 1
    syscall
    ret

; --------------------------
; print_uint
; 入力：RDI = 符号なし整数
; 出力：値を10進でstdoutに表示
; 処理概要：
;  1) スタック上にバッファを確保(最大20桁)
;  2) 10で割りながら1桁ずつ抽出
;  3) ASII文字にしてバッファに詰める(逆順)
;  4) 完成後、正しい順序でwrite
; --------------------------
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

; --------------------------
; print_int
; 入力：RDI = 符号付き整数
; 出力：値を10進でstdoutに表示
; 処理概要：
;  1) 負の数なら'-'を出力し、値を正にする
;  2) print_uintを呼び出す
; --------------------------
print_int:
    cmp rdi, 0
    jge .positive    ; rdi >= 0 ならそのままprint_uint

    ; --- 負の数の場合 ---
    ; '-' を一文字出力
    mov rax, 1
    mov rdi, 1
    mov rsi, minus_char
    mov rdx, 1
    syscall

    neg rdi         ; rdiを2の補数にして正に変換

.positive:
    ; 正の数をprint_uintに渡す
    call print_uint
    ret

; --------------------------
; read_char
; stdinから1文字だけ読み込む
; 
; --------------------------
read_char:
    sub rsp, 1
    mov rax, 0
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 1
    syscall

    cmp rax, 0
    je .eof

    mov rdi, rax
    call print_char
    ret
.eof
    add rsp, 1
    xor rax, rax
    ret

; --------------------------
; 
; --------------------------
read_word:
    ret

; --------------------------
; 
; --------------------------
parse_uint:
    xor rax, rax
    ret

; --------------------------
; 
; --------------------------
parse_int:
    xor rax, rax
    ret 


; --------------------------
; 
; --------------------------
string_equals:

; --------------------------
; 
; --------------------------
string_copy:
    ret
