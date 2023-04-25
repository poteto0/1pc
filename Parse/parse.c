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

// program = function*
Function *program() {
  Function head;
  head.next = NULL;
  Function *cur = &head;

  while (!at_eof()){
    cur->next = function();
    cur = cur->next;
  }
  return head.next;
}

// function = ident "(" ")" "{" stmt* "}"
Function *function() {
  locals = NULL;

  char *name = expect_ident(); // 関数名
  expect("(");
  expect(")");
  expect("{");

  Node head;
  head.next = NULL;
  Node *cur = &head;

  while(!consume("}")) {
    cur->next = stmt();
    cur = cur->next;
  }

  Function *fn = calloc(1, sizeof(Function));
  fn->name = name;
  fn->node = head.next;
  fn->locals = locals;
  return fn;
}

// プログラム1行単位
// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | expr ";"
//      | "{" stmt* "}"
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

  if(consume("while")){
    Node *node = new_node(ND_WHILE);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  // for(init = expr?; cond = expr?; inc = expr?) stmt
  if(consume("for")){
    Node *node = new_node(ND_FOR);
    expect("(");
    // ;でないとしたらexprが存在する
    if(!consume(";")){
      node->init = new_unary_node(ND_EXPR_STMT, expr());
      expect(";");
    }
    if(!consume(";")){
      node->cond = expr();
      expect(";");
    }
    if(!consume(")")){
      node->inc = new_unary_node(ND_EXPR_STMT, expr());
      expect(")");
    }
    node->then = stmt();
    return node;
  }

  // {...} block
  if(consume("{")){
    // 空のノードにつなげる
    Node head;
    head.next = NULL;
    Node *cur = &head;

    while (!consume("}")) {
      cur->next = stmt();
      cur = cur->next;
    }

    Node *node = new_node(ND_BLOCK);
    node->next = head.next;
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

// func-args = "(" ( assingn ("," assign)*)? ")"
Node *func_args(){
  if(consume(")")) // 引数なし
    return NULL;
  
  // 連結リストで変数を管理する
  Node *head = assign(); // 引数(変数)
  Node *cur = head;

  // "," が続く限り引数がある
  while(consume(",")){
    cur->next = assign();
    cur = cur->next;
  }
  expect(")");
  return head;
}

// primary = num 
//         | "(" expr ")"
//         | ident ( "(" ")"" )? => function without args
Node *primary() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if(consume("(")){
    Node *node = expr();
    expect(")");
    return node;
  }

  // variable or function
  Token *tok = consume_ident();
  if(tok){
    // function
    if(consume("(")){
      Node *node = new_node(ND_FUNCALL);
      node->funcname = strndup(tok->str, tok->len);
      node->args = func_args(); // 引数
      return node;
    }
    // variable
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