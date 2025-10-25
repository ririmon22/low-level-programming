=== 第1章 コンピュータアーキテクチャの基礎 ===

Q1. ハードウェアスタックのLIFO動作確認  
push命令で0x1234と0x5678をこの順序でスタックに積んだ後、2回popを実行し、raxとrbxに値が復元される様子を検証するNASMコードを記述せよ。  
・難易度: Easy  
・コード骨格:
```nasm
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
    syscall
```
・実行結果: プログラム終了時、raxには 0x5678、rbxには 0x1234 が格納されていること（gdbで確認）。  
・補足: ハードウェアスタックがLIFO（後入れ先出し）の原則に従うことを理解する。  
・参考ページ: p.21, 43  

---

Q2. RAXの部分アクセスと上位ビットのゼロ埋め効果の検証  
raxに初期値0xAAAAAAAABBBBBBBBを格納した後、その32ビット部分であるeaxに0xDEADBEEFを書き込む処理を記述し、最終的なraxの内容を確認せよ。  
・難易度: Medium  
・コード骨格:
```nasm
section .text
global _start
_start:
    ; 1. raxに初期値を設定
    mov rax, 0xAAAAAAAABBBBBBBB
    
    ; 2. eax（32ビット部分）に書き込み
    mov eax, 0xDEADBEEF
    
    ; raxの値を終了コードとして返す (rax -> rdi)
    mov rdi, rax
    mov rax, 60 ; exit syscall
    syscall
```
・実行結果: プログラム終了コード（$?）が 0x00000000DEADBEEF であること。  
・補足: Intel 64アーキテクチャでは、32ビットレジスタ（例：eax）への書き込みは、対応する64ビットレジスタ（例：rax）の上位32ビットをゼロで埋める（ゼロ拡張する）という特殊な振る舞いを引き起こす。  
・参考ページ: p.46, 51  

---

Q3. RFLAGS操作による条件ジャンプの検証  
cmp命令を使用して、比較結果が等しくない（ZF=0）状況を作り出し、その直後のjnz（Jump if Not Zero）命令によって意図したラベルに制御が移ることを確認するためのNASMコードを記述せよ。  
・難易度: Medium  
・コード骨格:
```nasm
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
```
・実行結果: プログラム終了コード（$?）が 0 であること。cmp命令がrflagsレジスタのZF（Zero Flag）を設定し、jnzがそれを利用することを確認する。  
・補足: cmp命令はオペランドの比較結果（引き算の結果）をrflagsに反映する。  
・参考ページ: p.17, 29  

---

Q4. スタック上のクオドワードデータのリトルエンディアン検証  
raxに0x1122334455667788を格納し、push raxを実行した後、スタック上のメモリ領域をgdbで覗き、リトルエンディアンで格納されていることを確認するNASMコードを記述せよ。  
・難易度: Hard  
・コード骨格:
```nasm
section .text
global _start
_start:
    mov rax, 0x1122334455667788
    push rax

    ; デバッガでスタック内容を確認するために無限ループ
    jmp $
```
・実行結果: gdbで停止後、x/8xb $rspを実行し、アドレスの低い方から高い方に向かって 0x88 0x77 0x66 0x55 0x44 0x33 0x22 0x11 の順に格納されていることを確認せよ。  
・補足: Intel 64はリトルエンディアンを採用しており、データの最下位バイト（LSB）が最もアドレスの低い位置に格納される。  
・参考ページ: p.21–22, 30, 44  

---

Q5. push byteによる符号拡張の検証  
NASMで、8ビット値である即値データ0x80をpush byte命令でスタックにプッシュし、その後pop raxでレジスタに戻すコードを記述せよ。スタックに格納される際に、値が64ビット幅で符号拡張されていることを確認せよ。  
・難易度: Hard  
・コード骨格:
```nasm
section .text
global _start
_start:
    ; 1. 8ビット値 0x80 (符号付きでは負) をプッシュ
    push byte 0x80
    
    ; 2. pop rax で復元
    pop rax
    
    ; raxの値を終了コードとして返す
    mov rdi, rax
    mov rax, 60
    syscall
```
・実行結果: プログラム終了コード（$?）が 0xFFFFFFFFFFFFFF80 となっていること。  
・補足: push命令は引数のサイズに関わらず、スタックには64ビット（8バイト）で格納するが、引数が小さい場合は符号拡張される。  
・参考ページ: p.25, 28  

---

出題数合計: 5問  
難易度分布: Easy 1 / Medium 2 / Hard 2  
章の学習目標: レジスタ操作、スタックのLIFO動作とメモリレイアウト（エンディアン）、レジスタの部分アクセス、および符号拡張の特殊な挙動を、アセンブリコードの実装を通じて体験的に理解する。

