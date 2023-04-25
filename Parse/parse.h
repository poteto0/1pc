#include "Node/node.h"
#include "LVar/lvarHelper.c"
#include "../Function/Function.h"
#include "../Tokenize/tokenize.c"

// ノードの作成
Node *new_node(NodeKind kind);
Node *new_binary_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_unary_node(NodeKind kind, Node *expr); // 左辺のみ
Node *new_num_node(int val);
Node *new_lvar_node(LVar *lvar); // 新しい変数を作る

Function *program();

Function *function();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *func_args();
Node *primary();
Node *unary();