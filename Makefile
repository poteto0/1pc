CFLAGS=-std=c11 -g -static

1pc: 1pc.c

test: 1pc
	./test/test.sh

clean:
	rm -f 1pc *.o *~ tmp*

.PHONY: test clean
