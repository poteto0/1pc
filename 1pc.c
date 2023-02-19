#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];

  printf(".globl main\n");
  printf("main:\n");
  // strolで読み込んだ後次のポインタを返す
  // これで順々に見れる
  printf("  mov x0, %ld\n", strtol(p, &p, 10));

  // 加減コンパイラ
  // 読み込みながら次の文字を返していくのを繰り返して式全体を読み込む
  while (*p) {
    if (*p == '+') {
      p++; // 1文字進める
      printf("  add x0, x0, %ld\n", strtol(p, &p, 10));
      continue;
    }

    if (*p == '-') {
      p++;
      printf("  sub x0, x0, %ld\n", strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "予期しない文字です: '%c'\n", *p);
    return 1;
  }


  printf("  ret\n");
  return 0;
}