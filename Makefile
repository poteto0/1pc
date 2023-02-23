CFLAGS=-std=c11 -g -static # コマンドラインオプション
SRCS=$(wildcard *.c) # 関数の引数にマッチするファイル名に展開される EX: 1pc.c
OBJS=$(SRCS:.c=.o) # .cを.oに書き換える

1pc: $(OBJS)
	$(CC) -o 1pc $(OBJS) $(LDFLAGS)

# カレントにある全てのファイルが1pc.hと依存関係にある
# -> 1pc.hを変更した場合全てを再コンパイル
$(OBJS): 1pc.h

test: 1pc
	./test/test.sh

clean:
	rm -f 1pc *.o *~ tmp*

.PHONY: test clean
