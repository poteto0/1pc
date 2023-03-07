#ifndef __NODE__STRUCT
#define __NODE__STRUCT

typedef struct Node Node;

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,      // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_LVAR,      // ローカル変数
  ND_NUM,       // 整数
  ND_ASSIGN,    // = (代入演算)
  ND_RETURN,    // "return"
  ND_EXPR_STMT, // 左辺だけ for文とかで使う
  // 制御構文
  ND_IF,     // "if"
  ND_WHILE,  // "while"
  ND_FOR,    // "for"
} NodeKind;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;   // ノードの型
  Node     *lhs;   // 左辺
  Node     *rhs;   // 右辺
  int      val;    // kindがND_NUMの場合のみ使う
  int      offset; // kindがND_LVARの場合のみ使う -> ベースポインタからのオフセット

  // "if" or "while" or "for" statement //
  Node *cond; // 条件文
  Node *then; // 条件式を満たしたら
  Node *els;
  Node *init; // 初期値
  Node *inc;  // 加算式
};

#endif
