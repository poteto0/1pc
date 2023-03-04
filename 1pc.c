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
  // プログラムを読み込むaaaaaaaaaaaaaaaaaaaaaaa
  program();

  // コードを出力する
  code_gen();

  return 0;
}