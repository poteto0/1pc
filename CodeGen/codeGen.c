#include "codeGen.h"

// アセンブリによるスタックマシン
void gen(Node *node) {
  // 数字|変数|代入演算の場合にはスタックマシンにプッシュする
  switch (node->kind) {
    case ND_NUM:
      gen_num_node(node->val);
      return;
    case ND_EXPR_STMT:
      gen(node->lhs);
      printf(" add sp, sp, 16\n");
      return;
    case ND_LVAR:
      gen_lvar_node(node);
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
    case ND_FUNCALL: { // こうしないと宣言と定義を別々にしないといけなくなる
      int nargs = 0; // 引数の数
      for(Node *arg = node->args; arg; arg = arg->next){
        gen(arg);
        nargs++;
      }
      for(int i=nargs-1; i>=0; i--){
        // 4つ以内であれば引数を変数で管理する
        if(i <= 8){
          printf(" ldr %s, [sp], 16\n", argreg[i]);
        }
        // TODO: 9つ以上はスタックにpushして管理する
        else{
          printf(" ldr x3, [sp], 16\n");
          printf(" str x3, [sp]\n");
        }
      }
      // 元のメモリアドレスを格納し、関数から戻ってくる
      printf(" str lr, [sp, -16]!\n");
      // 関数呼び出し
      printf(" bl %s\n", node->funcname);
      printf(" ldr lr, [sp], 16\n");
      printf(" str x0, [sp, -16]!\n");
      return;
    }
    case ND_RETURN: // returnが出てきたらそこで終了
      gen(node->lhs); // 左辺をgenして返り値を求める
      printf(" ldr x0, [sp], 16\n");
      printf(" b .Lreturn.%s\n", funcname); // returnへジャンプ
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
      gen_add();
      break;
    case ND_SUB:
      gen_sub();
      break;
    case ND_MUL:
      gen_mul();
      break;
    case ND_DIV:
      gen_div();
      break;
    case ND_EQ:
      gen_eq();
      break;
    case ND_NE:
      gen_ne();
      break;
    case ND_LT:
      gen_lt();
      break;
    case ND_LE:
      gen_le();
      break;
  }

  // 計算結果をスタックにpush
  printf(" str x0, [sp, -16]!\n");
}

void code_gen(Function *prog){
  // 関数を全て出力
  for(Function *fn = prog; fn; fn = fn->next) {
    // genで扱うからグローバル変数におく
    funcname = fn->name;

    // アセンブリの前半部分を出力
    printf(".globl %s\n", funcname);
    printf("%s:\n", funcname);

    prologue(fn->stack_size);

    // 1行づつコードを生成する
    for(Node *node = fn->node; node; node = node->next){
      // 抽象構文木を下りながらコード生成
      gen(node);
    }

    epilogue(funcname);

  }
  
}