#ifndef __CODE__HELPER
#define __CODE__HELPER

#include "../Parse/Node/node.h"

void load();
void store();

void gen_num_node(int num);
void gen_lvar_node(Node *node);

void gen_add();
void gen_sub();
void gen_mul();
void gen_div();
void gen_eq();
void gen_ne();
void gen_lt();
void gen_le();

void prologue(int stack_size);
void epilogue(char *name);

void gen_lval(Node *node);

#endif