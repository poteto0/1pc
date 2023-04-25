#include "codeGenHelper.h"

void gen_num_node(int num) {
  printf(" mov x0, %d\n", num);
  printf(" str x0, [sp, -16]! \n"); // 16バイトずつ
}
// 変数ノード
void gen_lvar_node(Node *node) {
  gen_lval(node);
  load();
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

void gen_add() {
  printf(" add  x0, x0, x1\n");
}
void gen_sub() {
  printf(" sub  x0, x0, x1\n");
}
void gen_mul() {
  printf(" mul  x0, x0, x1\n");
}
void gen_div() {
  printf(" sdiv x0, x0, x1\n");
}
void gen_eq() {
  printf(" cmp  x0, x1\n");
  printf(" cset x0, eq\n");
}
void gen_ne() {
  printf(" cmp  x0, x1\n");
  printf(" cset x0, ne\n");
}
void gen_lt() {
  printf(" cmp  x0, x1\n");
  printf(" cset x0, lt\n");
}
void gen_le() {
  printf(" cmp  x0, x1\n");
  printf(" cset x0, le\n");
}

void prologue(int stack_size){
  printf(" mov x29, sp\n");
  printf(" str x29, [sp, -16]!\n");
  printf(" sub sp, sp, %d\n", stack_size);
}
void epilogue(char *name){
  printf(".Lreturn.%s:\n", name);
  printf(" mov sp, x29\n");
  printf(" ldr x29, [sp], 16\n");
  printf(" ret\n");
}

// 左辺が変数の場合
// それ以外は無効な式 EX: 1 = 2
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf(" mov x0, x29\n");
  printf(" sub x0, x0, %d\n", node->offset);
  printf(" str x0, [sp, -16]!\n");
}