#ifndef __TOKENIZE
#define __TOKENIZE

#include "Token/token.h"
#include "Error/error.c"

// 入力プログラム全体の文字列
char *user_input;

// 現在着目しているトークン
Token *token;

bool consume(char* op);
Token *consume_ident();

char *strndup(char *p, int len);

void expect(char* op);

int expect_number();
char *expect_ident();

bool at_eof();

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

// 複数文字トークンの比較
bool startswith(char *p, char *q);

// 予約されている制御構文との比較 -> tokenを返す
char *starts_with_reserved(char *p);

// アルファベット | _
bool is_alpha(char c);
// 数字 | アルファベット | _
bool is_alnum(char c);

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p);

#endif