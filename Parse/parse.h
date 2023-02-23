#include "Node/node.h"
#include "../Tokenize/tokenize.c"

// セミコロン区切りで複数の式をかけるようにしたので、パースの結果として複数のノードを保存しておく
Node *code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();