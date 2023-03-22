#include "codeGen.h"

int labelseq = 0;

// 左辺が変数の場合
// それ以外は無効な式 EX: 1 = 2
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf(" mov x0, x29\n");
  printf(" sub x0, x0, %d\n", node->offset);
  printf(" str x0, [sp, -16]!\n");
}

void load() {
  // 変数: x0をアドレスとみなして値をロードしてx0に保存する
  printf(" ldr x0, [sp], 16\n");
  printf(" ldr x0, [x0]\n");
  printf(" str x0, [sp, -16]!\n");
}

void store() {
  printf(" ldr x1, [sp], 16\n");
  printf(" ldr x0, [sp], 16\n");
  // 変数への代入
  // x0をアドレスとみなしてx1の値をストアする
  printf(" str x1, [x0]\n");
  printf(" str x1, [sp, -16]!\n");
}

// アセンブリによるスタックマシン
void gen(Node *node) {
  // 数字|変数|代入演算の場合にはスタックマシンにプッシュする
  switch (node->kind) {
    case ND_NUM:
      printf(" mov x0, %d\n", node->val);
      printf(" str x0, [sp, -16]! \n"); // 16バイトずつ
      return;
    case ND_EXPR_STMT:
      gen(node->lhs);
      //printf(" add sp, sp, 16\n");
      return;
    case ND_LVAR:
      gen_lval(node);
      load();
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      store();
      return;
    case ND_IF: {
      // 関数名をラベリングする
      // 呼び出しの度に違う名前になる
      int seq = labelseq++;
      if(node->els){
        gen(node->cond);
        printf(" ldr x0, [sp], 16\n");
        printf(" cmp x0, 0\n"); // falseと比較
        printf(" b.eq .Lelse%d\n", seq); // 等しい場合
        gen(node->then); // trueならここを通ってendへ飛ぶ
        printf(" b .Lend%d\n", seq);
        printf(".Lelse%d:\n", seq); // falseならelseへ飛ぶ
        gen(node->els);
        printf(".Lend%d:\n", seq);
      }else{
        gen(node->cond); // 条件式
        printf(" ldr x0, [sp], 16\n");
        printf(" cmp x0, 0\n");
        printf(" b.eq .Lend%d\n", seq);
        gen(node->then);
        printf(".Lend%d:\n", seq);
      }
      return;
    }
    case ND_WHILE: {
      int seq = labelseq++;
      printf(".Lbegin%d:\n", seq);
      gen(node->cond); // 条件式
      printf(" ldr x0, [sp], 16\n");
      printf(" cmp x0, 0\n");
      printf(" b.eq .Lend%d\n", seq); // falseならendに飛ぶ
      gen(node->then);
      printf(" b .Lbegin%d\n", seq); // 最初に戻る
      printf(".Lend%d:\n", seq);
      return;
    }
    case ND_FOR: {
      int seq = labelseq++;
      if(node->init) gen(node->init);
      printf(".Lbegin%d:\n", seq);
      if(node->cond){
        gen(node->cond); // 条件式
        printf(" ldr x0, [sp], 16\n");
        printf(" cmp x0, 0\n");
        printf(" b.eq .Lend%d\n", seq); // falseならLendへ
      }
      gen(node->then); // for文の中を実行
      if(node->inc) gen(node->inc); // 加算式を実行
      printf(" b .Lbegin%d\n", seq);
      printf(".Lend%d:\n", seq);
      return;
    }
    // {...} block
    case ND_BLOCK:
      // 連結リスト内を全て実行する。
      for (Node *n = node->next; n; n = n->next)
        gen(n);
      return;
    case ND_FUNCALL:
      printf(" str lr, [sp, -16]!\n");
      // 呼び出し自体は成功している
      printf(" bl %s\n", node->funcname);
      printf(" ldr lr, [sp], 16\n");
      //printf(" ldr x0, [sp]\n");
      // ここでx0がおかしい？
      // w0やr0を試したけどダメだった。
      // printf(" mov x0, 30\n"); 30が返ると思ったけど違う？
      // 関数から返って来れないのでは？
      // testでreturn 1をやっても実行できなかったので、恐らくはそうなっている
      // 返ってくる時にメモリ外アクセスしてしまう？
      printf(" str x0, [sp, -16]!\n");
      return;
    case ND_RETURN: // returnが出てきたらそこで終了
      gen(node->lhs); // 左辺をgenして返り値を求める
      printf(" ldr x0, [sp], 16\n");
      printf(" b .Lreturn\n"); // returnへジャンプ
      return;
  }

  // 左辺と右辺をそれぞれ処理する
  gen(node->lhs);
  gen(node->rhs);

  // 左辺と右辺をポップする
  printf(" ldr x1, [sp], 16\n"); // 読み込んでから16ビット送る
  printf(" ldr x0, [sp], 16\n");

  // 四則演算
  switch (node->kind) {
    case ND_ADD:
      printf(" add  x0, x0, x1\n");
      break;
    case ND_SUB:
      printf(" sub  x0, x0, x1\n");
      break;
    case ND_MUL:
      printf(" mul  x0, x0, x1\n");
      break;
    case ND_DIV:
      printf(" sdiv x0, x0, x1\n");
      break;
    case ND_EQ:
      printf(" cmp  x0, x1\n");
      printf(" cset x0, eq\n");
      break;
    case ND_NE:
      printf(" cmp  x0, x1\n");
      printf(" cset x0, ne\n");
      break;
    case ND_LT:
      printf(" cmp  x0, x1\n");
      printf(" cset x0, lt\n");
      break;
    case ND_LE:
      printf(" cmp  x0, x1\n");
      printf(" cset x0, le\n");
      break;
  }

  // 計算結果をスタックにpush
  printf(" str x0, [sp, -16]!\n");
}

void code_gen(){
  // アセンブリの前半部分を出力
  printf(".globl main, foa\n");
  printf("foa:\n");
  printf(" mov x0, 1\n");
  printf(" ret\n");
  printf("main:\n");

  // Prologue
  printf(" mov x29, sp\n");
  printf(" sub sp, sp, 416\n");

  // 1行づつコードを生成する
  for(int i=0; code[i]; i++){
    // 抽象構文木を下りながらコード生成
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf(" ldr x0, [sp], 16\n");
  }

  // Epilogue
  printf(".Lreturn:\n");
  printf(" mov sp, x29\n");
  printf(" ldr x29, [sp], 16\n");
  printf(" ret\n");
}