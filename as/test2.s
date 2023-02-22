.globl main
main:
 mov x0, 11
 mov x1, 22
 cmp x0, x1
 cset x0, lt
 ret
