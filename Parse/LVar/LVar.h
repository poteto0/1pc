#ifndef __LVAR
#define __LVAR

// パーサで変数型のトークンが出現すると、
// localsを辿って既存の変数かどうか確認する
// 既存の変数ならばLVarのoffsetを利用する

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next;  // 次の変数かNULL
  char *name;  // 変数の名前
  int  len;    // 名前の長さ
  int  offset; // RBPからのオフセット
};

// 既存の変数を連結リストで管理する
LVar *locals;

#endif