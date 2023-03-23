#include "../Parse/Node/node.h"

int labelseq = 0;
// 引数の格納先(8つまで)をリストアップする
char *argreg[] = {"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7"};

// アセンブリによるスタックマシン
void gen(Node *node);

void load();
void store();

void gen_lval(Node *node);

void code_gen();