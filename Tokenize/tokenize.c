#include "tokenize.h"

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char* op) {
  if((token->kind != TK_RESERVED && token->kind != TK_RETURN) ||
     strlen(op) != token->len || // 文字数チェック
     memcmp(token->str, op, token->len) // 先頭から文字数分チェックする(==0 でtrue)
    )
    return false;
  token = token->next;
  return true;
}

// consumeの文字ver
Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char* op) {
  if(token->kind != TK_RESERVED || 
     strlen(op) != token->len || // 文字数チェック
     memcmp(token->str, op, token->len) // 先頭から文字数分チェックする(==0 でtrue)
    )
    error_at(user_input, token->str, "'%c'ではありません", op);
  token = token->next;
}

// 終端トークンかどうか
bool at_eof() {
  return token->kind == TK_EOF;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if(!at_eof()){
    if (token->kind != TK_NUM)
      error_at(user_input, token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
  }
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token)); // メモリの確保
  tok->kind = kind;
  tok->str  = str;
  tok->len  = len;
  cur->next = tok;
  return tok;
}

// 複数文字トークンの比較 → 2文字分比較したいからここで定義
bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

// アルファベット | _
bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// 数字 | アルファベット | _
bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  // ダミーのヘッドに繋げていく
  // こうすることで実装が楽になる
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p){
    // 空白文字をスキップ
    if(isspace(*p)){
      p++;
      continue;
    }

    // returnトークン                変数名かどうか判断する EX: return x
    if (startswith(p, "return") && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    // 複数記号トークン
    if(startswith(p, "==") || startswith(p, "!=") ||
       startswith(p, "<=") || startswith(p, ">=")){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2; // 2文字送る
      continue;
    }

    // 単数記号トークン
    if(strchr("+-*/()<>=;", *p)){
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // 変数
    if (is_alpha(*p)) {
      // 変数の場合には変数が終わるまで文字を送る
      char *q = p++;
      while (is_alnum(*p))
        p++;
      cur = new_token(TK_IDENT, cur, q, p - q);
      continue;
    }

    // 文字が数字の場合にはvalにその数字をセットする
    if (isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 0);
      // 数字によって桁数が違うため、1回0で初期化してから計算する
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(user_input, p, "トークナイズできません");
  }

  // 全部終わったら終了文字としてTokenに記録
  new_token(TK_EOF, cur, p, 0);
  return head.next; // これはつまりcurを返してる?
}