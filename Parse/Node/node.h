#ifndef __NODE__STRUCT
#define __NODE__STRUCT

typedef struct Node Node;

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_LVAR,   // ローカル変数
  ND_NUM,    // 整数
  ND_ASSIGN, // = (代入演算)
  ND_RETURN, // "return"
} NodeKind;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;   // ノードの型
  Node     *lhs;   // 左辺
  Node     *rhs;   // 右辺
  int      val;    // kindがND_NUMの場合のみ使う
  int      offset; // kindがND_LVARの場合のみ使う -> ベースポインタからのオフセット
};

#endif
