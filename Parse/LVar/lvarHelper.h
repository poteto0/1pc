#include "LVar.h"
#include "../../Tokenize/Token/token.h"

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token * tok);

// 変数を連結リストにpushする
LVar *push_lvar(Token *tok);