#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Token/token.c"
#include "AbstSyntaxTree/node.c"
#include "Error/error.c"

// 入力プログラム全体の文字列
char *user_input;

// 現在着目しているトークン
Token *token;

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char* op) {
  if(token->kind != TK_RESERVED || 
     strlen(op) != token->len || // 文字数チェック
     memcmp(token->str, op, token->len) // 先頭から文字数分チェックする(==0 でtrue)
    )
    return false;
  token = token->next;
  return true;
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

    // 複数記号トークン
    if(startswith(p, "==") || startswith(p, "!=") ||
       startswith(p, "<=") || startswith(p, ">=")){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2; // 2文字送る
      continue;
    }

    // 単数記号トークン
    if(strchr("+-*/()<>", *p)){
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // 文字が数字の場合にはvalにその数字をセットする
    if (isdigit(*p)) {
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();

// expr = equality
Node *expr(){
  return equality();
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for(;;){
    if(consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if(consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
  Node *node = add();

  for(;;){
    if(consume("<"))
      node = new_node(ND_LT, node, add());
    else if(consume("<="))
      node = new_node(ND_LE, node, add());
    else if(consume(">")) // < の右辺と左辺を逆転
      node = new_node(ND_LT, add(), node);
    else if(consume(">=")) // <= の右辺と左辺を逆転
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
Node *add(){
  Node *node = mul();

  for(;;){
    if(consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if(consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// mull = primary ("*" primary | "/" primary)*
Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? unary | primary
Node *unary() {
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

// primary = num | "(" expr ")"
Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

// アセンブリによるスタックマシン
void gen(Node *node) {
  // 数字の場合にはスタックマシンにプッシュする
  if (node->kind == ND_NUM) {
    printf(" mov x0, %d\n", node->val);
    printf(" str x0, [sp, -16]! \n"); // 16バイトずつ
    return;
  }

  // 左辺と右辺をそれぞれ処理する
  gen(node->lhs);
  gen(node->rhs);

  // 左辺と右辺をポップする
  printf(" ldr x0, [sp], 16\n");
  printf(" ldr x1, [sp], 16\n");

  // 四則演算
  switch (node->kind) {
    case ND_ADD:
      printf(" add  x0, x1, x0\n");
      break;
    case ND_SUB:
      printf(" sub  x0, x1, x0\n");
      break;
    case ND_MUL:
      printf(" mul  x0, x1, x0\n");
      break;
    case ND_DIV:
      printf(" sdiv x0, x1, x0\n");
      break;
    case ND_EQ:
      printf(" cmp  x1, x0\n");
      printf(" cset x0, eq\n");
      break;
    case ND_NE:
      printf(" cmp  x1, x0\n");
      printf(" cset x0, ne\n");
      break;
    case ND_LT:
      printf(" cmp  x1, x0\n");
      printf(" cset x0, lt\n");
      break;
    case ND_LE:
      printf(" cmp  x1, x0\n");
      printf(" cset x0, le\n");
      break;
  }

  // 計算結果をスタックにpush
  printf(" str x0, [sp, -16]!\n");
}


int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // 文字列全体を取得
  user_input = argv[1];
  // トークナイズする
  token = tokenize(user_input);
  // 抽象構文木を作成する
  // 構文を木構造に記述して、演算などの順序を表現する
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".globl main\n");
  printf("main:\n");

  // 式の最初は数でなければならないので、それをチェックして
  // 最初のmov命令を出力
  printf(" mov x0, %d\n", expect_number());

  /*
  // `+ <数>`あるいは`- <数>`というトークンの並びを消費しつつ
  // アセンブリを出力
  while (!at_eof()) {
    // [+]かどうかチェックして、数字を読み取って加算
    if (consume('+')) {
      printf(" add x0, x0, %d\n", expect_number());
      continue;
    }

    // [-]かどうかチェックして数字を読み取って減算数
    expect('-');
    printf(" sub x0, x0, %d\n", expect_number());
  }
  */

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数からの返り値とする
  printf(" ldr x0, [sp], 16\n");
  printf(" ret\n");
  return 0;
}