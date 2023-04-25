#ifndef __FUNCTION
#define __FUNCTION

#include "../Parse/LVar/LVar.h"

typedef struct Function Function;

struct Function {
  Function *next;
  char *name;
  Node *node;
  LVar *locals;   // 引数をローカル変数に代入する
  int stack_size; // 関数で利用する領域の容量
};

#endif