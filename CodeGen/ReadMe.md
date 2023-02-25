# gramatics in arm64
I write by arm64

## comparison
```s
cmp x0, x1  // compare
cset x0, eq // ==
cset x0, ne // !=
cset x0, lt // <
cset x0, le // <=
```

## push and pop
```s
// push
str x0, [sp, -16]! // sp-=16; x0 = *sp;
// pop
ldr x0, [sp], 16 // *sp = x0; sp += 16;
```

## store and load
```s
// store
str x0, [sp] // *sp = x0;
// load
ldr x0, [sp] // x0 = *sp;
```

## store with offset
```s
str x0, [sp, -offset]! // sp -= offset; x0 = *sp;
```

## prologue and epilogue
I think they are not necessary on arm64.

reference:
https://www.mztn.org/dragon/arm6405str.html