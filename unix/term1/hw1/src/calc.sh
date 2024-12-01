#!/usr/bin/env sh

if [ $# -eq 0 ]; then
    read -r expression
    result=$(( expression ))
else
    result=$(( $@ ))
fi

echo $result
