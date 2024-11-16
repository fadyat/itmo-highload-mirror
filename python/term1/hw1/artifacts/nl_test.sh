#!/bin/zsh

exec 2>/dev/null

function assert_nl {
  local file="$1"
  nl -b a $file > nl.out
  python nl.py $file > np.py.out
  diff nl.out np.py.out > nl.diff

  [ -s nl.diff ] && { echo "Test nl: $file failed"; cat nl.diff; } || echo "Test nl: $file passed"
}

assert_nl .gitignore
assert_nl pyproject.toml
assert_nl nl.py
assert_nl not_exists

rm -f nl.out np.py.out nl.diff