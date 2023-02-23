#include "../Parse/Node/node.h"

// アセンブリによるスタックマシン
void gen(Node *node);

void load();
void store();

void gen_lval(Node *node);

void code_gen();