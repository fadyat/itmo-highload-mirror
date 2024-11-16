#!/bin/zsh

exec 2>/dev/null

function assert_tail {
  local file=$1

  tail -n 10 "$@" > tail.out
  python tail.py "$@" > tp.py.out
  diff tail.out tp.py.out > tail.diff

  [ -s tail.diff ] && { echo "Test tail: "$@" failed"; cat tail.diff; } || echo "Test tail: "$@" passed"

  cat "$@" | tail -n 17 > tail.out
  cat "$@" | python tail.py > tp.py.out
  diff tail.out tp.py.out > tail.diff

  [ -s tail.diff ] && { echo "Test tail: "$@" failed"; cat tail.diff; } || echo "Test tail: "$@" passed"
}

assert_tail .gitignore
assert_tail pyproject.toml
assert_tail tail.py
assert_tail .gitignore pyproject.toml tail.py
assert_tail .gitignore not_exists . tail.py

rm -f tail.out tp.py.out tail.diff
