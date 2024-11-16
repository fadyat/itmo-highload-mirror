#!/bin/zsh

exec 2>/dev/null

function assert_wc() {
    wc "$@" > wc.out
    python wc.py "$@" > wc.py.out
    diff wc.out wc.py.out > wc.diff

    [ -s wc.diff ] && { echo "Test wc: "$@" failed"; cat wc.diff; } || echo "Test wc: "$@" passed"
}

assert_wc .gitignore
assert_wc pyproject.toml
assert_wc wc.py
assert_wc .gitignore pyproject.toml wc.py
assert_wc .gitignore not_exists . wc.py

rm -f wc.out wc.py.out wc.diff