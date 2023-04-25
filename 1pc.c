#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parse/parse.c"
#include "CodeGen/codeGen.c"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // 文字列全体を取得
  user_input = argv[1];
  // トークナイズする
  token = tokenize(user_input);
  // プログラムを読み込むa
  Function *prog = program();

  // 関数内のローカル変数にオフセットを与える
  // 関数内でローカル変数を区別できるようにする
  for(Function *fn = prog; fn; fn = fn->next) {
    int offset = 0;

    // オフセットは16
    for(LVar *lvar = prog->locals; lvar; lvar = lvar->next) {
      offset += 16;
      lvar->offset = offset;
    }

    fn->stack_size = offset;
  }

  // コードを出力する
  code_gen(prog);

  return 0;
}