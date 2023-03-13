#include "Node/node.h"
#include "LVar/lvarHelper.c"
#include "../Tokenize/tokenize.c"

// セミコロン区切りで複数の式をかけるようにしたので、パースの結果として複数のノードを保存しておく
Node *code[100];

// ノードの作成
Node *new_node(NodeKind kind);
Node *new_binary_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_unary_node(NodeKind kind, Node *expr); // 左辺のみ
Node *new_num_node(int val);
Node *new_lvar_node(LVar *lvar); // 新しい変数を作る

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *primary();
Node *unary();