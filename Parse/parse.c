#include "parse.h"

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

// program = stmt*
Node *program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt(); // 各行を保存する
  code[i] = NULL;
}

// stmt = expr ";"
// プログラム1行単位
Node *stmt(){
  Node *node = expr();
  expect(";");
  return node;
}

// expr = assign
Node *expr(){
  return assign();
}

// assign = equality ("=" assign)
// EX: a = 1, a = b = 1
Node *assign(){
  Node *node = equality();
  if(consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
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

  for(;;){
    if(consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if(consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? unary | primary
Node *unary() {
  if(consume("+"))
    return unary();
  if(consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

// primary = num | "(" expr ")"
Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if(consume("(")){
    Node *node = expr();
    expect(")");
    return node;
  }

  // 変数ノード
  Token *tok = consume_ident();
  if(tok){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8; // aから何文字離れてるか * 8
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}