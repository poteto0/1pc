#include "codeGen.h"

// 左辺が変数の場合
// それ以外は無効な式 EX: 1 = 2
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf(" mov x0, base\n");
  printf(" sub x0, %d\n", node->offset);
  printf(" str x0, [sp, -16]!\n");
}

void load() {
  printf(" ldr x0, [sp], 16\n");
  // 変数: x0をアドレスとみなして値をロードしてx0に保存する
  printf(" mov x0, [x0]\n");
  printf(" str x0, [sp, -16]! \n"); // 16バイトずつ
}

void store() {
  printf(" ldr x1, [sp], 16\n");
  printf(" ldr x0, [sp], 16\n");
  // 変数への代入
  // x0をアドレスとみなしてx1の値をストアする
  printf(" mov [x0], x1\n");
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
    case ND_LVAR:
      //gen_lval(node);
      //load();
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      store();
      return;
  }

  // 左辺と右辺をそれぞれ処理する
  gen(node->lhs);
  gen(node->rhs);

  // 左辺と右辺をポップする
  printf(" ldr x1, [sp], 16\n");
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
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  // 変数26個分の領域を確保する
  //printf(" str base, [sp, -16]!\n"); // ベースレジスタをpush
  //printf(" mov base, x0\n"); // baseをx0までずらす
  //printf(" sub x0, 208\n"); // 26*8領域を確保

  // 1行づつコードを生成する
  for(int i=0; code[i]; i++){
    // 抽象構文木を下りながらコード生成
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf(" ldr x0, [sp], 16\n");
  }

  // エピローグ
  // 最後の式の結果がx0に残っているため、それを返り値とする
  //printf(" mov x0, base\n");
  //printf(" ldr base, [sp], 16\n");
  printf(" str x0, [sp, -16]!\n");
  printf(" ret\n");
}