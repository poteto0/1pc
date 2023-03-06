#include "parse.h"

// 左辺と右辺を設定しない
Node *new_node(NodeKind kind){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// 左辺のみ設定する
Node *new_unary_node(NodeKind kind, Node *lhs){
  Node *node = new_node(kind);
  node->lhs = lhs;
  return node;
}

Node *new_num_node(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

// 新しい変数を作る
Node *new_lvar_node(LVar *lvar){
  Node *node = new_node(ND_LVAR);
  node->offset = lvar->offset;
  return node;
}

// program = stmt*
Node *program() {
  locals = NULL;
  int i = 0;
  while (!at_eof())
    code[i++] = stmt(); // 各行を保存する
  code[i] = NULL;
}

// プログラム1行単位
// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | expr ";"
Node *stmt(){
  if(consume("return")){
    Node *node = new_unary_node(ND_RETURN, expr());
    expect(";");
    return node;
  }

  if(consume("if")){
    Node *node = new_node(ND_IF);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if(consume("else"))
      node->els = stmt();
    return node;
  }
  
  // returnでないならexpr
  Node* node = expr();
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
    node = new_binary_node(ND_ASSIGN, node, assign());
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for(;;){
    if(consume("=="))
      node = new_binary_node(ND_EQ, node, relational());
    else if(consume("!="))
      node = new_binary_node(ND_NE, node, relational());
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
  Node *node = add();

  for(;;){
    if(consume("<"))
      node = new_binary_node(ND_LT, node, add());
    else if(consume("<="))
      node = new_binary_node(ND_LE, node, add());
    else if(consume(">")) // < の右辺と左辺を逆転
      node = new_binary_node(ND_LT, add(), node);
    else if(consume(">=")) // <= の右辺と左辺を逆転
      node = new_binary_node(ND_LE, add(), node);
    else
      return node;
  }
}

// add = mul ("+" mul | "-" mul)*
Node *add(){
  Node *node = mul();

  for(;;){
    if(consume("+"))
      node = new_binary_node(ND_ADD, node, mul());
    else if(consume("-"))
      node = new_binary_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// mull = primary ("*" primary | "/" primary)*
Node *mul() {
  Node *node = unary();

  for(;;){
    if(consume("*"))
      node = new_binary_node(ND_MUL, node, unary());
    else if(consume("/"))
      node = new_binary_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? unary | primary
Node *unary() {
  if(consume("+"))
    return unary();
  if(consume("-"))
    return new_binary_node(ND_SUB, new_num_node(0), unary());
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
    node->offset = (tok->str[0] - 'a' + 1) * 16; // aから何文字離れてるか * 16
    return node;

    // 既存の変数でなければNULL
    LVar *lvar = find_lvar(tok);

    if(!lvar)
      lvar = push_lvar(tok); // 新しい変数を作ってlocalsにpush
    return new_lvar_node(lvar); // 既存の変数ならoffsetを受け継いで新たなノードとする
  }

  // そうでなければ数値ノード
  return new_num_node(expect_number());
}