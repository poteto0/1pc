int labelseq = 0;
char *funcname;
// 引数の格納先(8つまで)をリストアップする
char *argreg[] = {"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7"};

#include "codeGenHelper.c"

// アセンブリによるスタックマシン
void gen(Node *node);

void code_gen(Function *prog);