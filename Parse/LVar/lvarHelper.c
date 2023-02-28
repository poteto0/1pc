#include "lvarHelper.h"

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok){
  for (LVar *lvar = locals; lvar; lvar = lvar->next)
    if (lvar->len == tok->len && !memcmp(tok->str, lvar->name, lvar->len))
      return lvar;
  return NULL;
}

// 変数を連結リストにpushする
LVar *push_lvar(Token *tok){
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals; // 頭につなげてく
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->offset = locals->offset + 16; // 変数ごとにoffsetを16ずらす
  locals = lvar;
  return lvar;
}