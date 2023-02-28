#ifndef __TOKEN__STRUCT
#define __TOKEN__STRUCT

typedef struct Token Token;

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子トークン
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // "return"
} TokenKind;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token*    next; // 次の入力トークン
  int       val;  // kindがTK_NUMの場合、その数値
  char*     str;  // トークン文字列
  int       len;  // 文字列の長さ → ==などの2文字を扱えるようにする
};

#endif