#include "Token/token.h"
#include "Error/error.c"

// 入力プログラム全体の文字列
char *user_input;

// 現在着目しているトークン
Token *token;

bool consume(char* op);
Token *consume_ident();

void expect(char* op);

int expect_number();

bool at_eof();

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

bool startswith(char *p, char *q);

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p);