assert() {
  expected="$1"
  input="$2"

  ./1pc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
# 単項演算
assert 1 '-11 + 2*6;'
# 比較演算
assert 1 '11 == 11;'
assert 0 '11 != 11;'
assert 1 '11 < 22;'
assert 1 '11 <= 22;'
assert 0 '11 > 22;'
assert 0 '11 >= 22;'
# 変数(1文字)
assert 5 ' b =  1;'
#assert 3 'a=1; b=2; a+b;'

echo OK