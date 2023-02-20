#ifndef __ERROR__METHOD
#define __ERROR__METHOD

// エラー報告と同時にエラー箇所を報告する
void error_at(char* user_input, char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input; // 文字列の長さからエラー箇所を取得
  fprintf (stderr, "%s\n", user_input);
  fprintf (stderr, "%*s",  pos, " "); // pos個の空白を出力
  fprintf (stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf (stderr, "\n");
  //exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

#endif