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
    mov rax, rdi
    mov rdi, rsp
    push 0
    sub rsp, 16
    
    dec rdi
    mov r8, 10

.loop:
    xor rdx, rdx
    div r8
    or  dl, 0x30
    dec rdi 
    mov [rdi], dl
    test rax, rax
    jnz .loop 
   
    call print_string
    
    add rsp, 24
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
    test rdi, rdi
    jns print_uint
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi
    jmp print_uint

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
    movzx rax, byte [rsp] ; 読んだ文字コード
    add rsp, 1
    ret

.eof:
    add rsp, 1
    xor rax, rax        ; 0 を返す
    ret

; --------------------------
; read_word
; 入力：
;   RDI = バッファ先頭
;   RSI = バッファサイズ
; 出力：
;   RAX = 読み込んだ文字数（終端0は含まない）
; 処理概要：
;  1) 空白類（スペース, 改行, タブ）を読み飛ばす
;  2) 非空白が始まったらそこから単語読み取り開始
;  3) バッファ上限を過ぎたら終了
;  4) 空白に再遭遇したら終了
;  5) 最後に終端0を書き込む
; --------------------------
read_word:
    push r14
    push r15
    xor r14, r14 
    mov r15, rsi
    dec r15

    .A:
    push rdi
    call read_char
    pop rdi
    cmp al, ' '
    je .A
    cmp al, 10
    je .A
    cmp al, 13
    je .A 
    cmp al, 9 
    je .A
    test al, al
    jz .C

    .B:
    mov byte [rdi + r14], al
    inc r14

    push rdi
    call read_char
    pop rdi
    cmp al, ' '
    je .C
    cmp al, 10
    je .C
    cmp al, 13
    je .C 
    cmp al, 9
    je .C
    test al, al
    jz .C
    cmp r14, r15
    je .D

    jmp .B

    .C:
    mov byte [rdi + r14], 0
    mov rax, rdi 
   
    mov rdx, r14 
    pop r15
    pop r14
    ret

    .D:
    xor rax, rax
    pop r15
    pop r14
    ret
    push rbx

    mov rbx, rdi      ; rbx = buffer head
    xor rdx, rdx      ; rdx = length = 0

   
; --------------------------
; parse_uint
; 入力：RDI = 数字列の先頭
; 出力：
;   RAX = 読み取った整数
;   RDX = 消費した文字数
; 処理概要：
;  1) 数字でない文字が来るまでループ
;  2) '0'〜'9' を数値に変換
;  3) RAX = RAX*10 + 数値
;  4) 読んだ文字数を RDX でカウント
; --------------------------
parse_uint:
    mov r8, 10
    xor rax, rax
    xor rcx, rcx
.loop:
    movzx r9, byte [rdi + rcx] 
    cmp r9b, '0'
    jb .end
    cmp r9b, '9'
    ja .end
    xor rdx, rdx 
    mul r8
    and r9b, 0x0f
    add rax, r9
    inc rcx 
    jmp .loop 
    .end:
    mov rdx, rcx
    ret

; --------------------------
; parse_int
; 入力：RDI = 数字 or "-数字"
; 出力：
;   RAX = 結果の整数（負もOK）
;   RDX = 読んだ文字数
; 処理概要：
;  1) 先頭が '-' ならフラグを立て、1文字進める
;  2) parse_uint を呼んで絶対値部分を読む
;  3) '-' だった場合は RAX を neg する
; --------------------------
 parse_int:
    mov al, byte [rdi]
    cmp al, '-'
    je .signed
    jmp parse_uint
.signed:
    inc rdi
    call parse_uint
    neg rax
    test rdx, rdx
    jz .error

    inc rdx
    ret

    .error:
    xor rax, rax
    ret 

; --------------------------
; string_equals
; 入力：
;   RDI = 文字列1
;   RSI = 文字列2
; 出力：
;   RAX = 1（等しい）
;   RAX = 0（異なる）
; 処理概要：
;  1) 両方の文字を1バイトずつ比較
;  2) 違えば 0 を返す
;  3) 両方が 0 なら等しい → 1 を返す
; --------------------------
string_equals:
    xor r8, r8
    
.compare:
    mov bl, [rdi + r8]
    mov cl, [rsi + r8]

    cmp bl, cl
    jne .not_equal

    cmp bl, 0
    je .equal

    inc r8
    jmp .compare


.not_equal:
    mov rax, 0
    ret
    
.equal:
    mov rax, 1
    ret
; --------------------------
; string_copy
; 入力：
;   RDI = コピー先
;   RSI = コピー元
; 出力：
;   RAX = コピー先アドレス（つまり RDI）
; 処理概要：
;  1) [RDI] = [RSI] と1バイトずつコピー
;  2) 終端0もコピーする
; --------------------------
string_copy:

    push rdi
    push rsi
    push rdx
    call string_length
    pop rdx
    pop rsi
    pop rdi

    cmp rax, rdx
    jae .too_long  ; we also need to store null-terminator
    
    push rsi 

        .loop: 
        mov dl, byte[rdi]
        mov byte[rsi], dl
        inc rdi
        inc rsi
        test dl, dl
        jnz .loop 

    pop rax 
    ret

    .too_long:
    xor rax, rax
    ret