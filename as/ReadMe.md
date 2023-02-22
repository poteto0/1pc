# arm64記法へ書き下し
## pushとpop
```arm64
mov x0, 2
mov x1, 3

// 左辺と右辺をプッシュ
str x0, [sp, -16]!
str x1, [sp, -16]!

// 左辺と右辺をx0とx1にポップして足す
ldr x0, [sp], 16
ldr x1, [sp], 16
mul x0, x0, x1

// 足した結果をスタックにプッシュ
str x0, [sp, -16]!

mov x0, 4
mov x1, 5

str x0, [sp, -16]!
str x1, [sp, -16]!
ldr x0, [sp], 16
ldr x1, [sp], 16
mul x0, x0, x1
// 足した結果をスタックにプッシュ
str x0, [sp, -16]!

ldr x0, [sp], 16
ldr x1, [sp], 16
add x0, x0, x1
str x0, [sp, -16]!
```
注意) x0とw0にアクセスしようとするとエラー

## 比較
```
cmp x0, x1  // compare
cset x0, eq // ==
cset x0, ne // !=
cset x0, lt // <
cset x0, le // <=
```

arm64
https://zenn.dev/kagamoc/scraps/8d4ab7157e0529