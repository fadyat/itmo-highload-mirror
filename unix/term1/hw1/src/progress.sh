#!/usr/bin/env sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 <log_file>"
    exit 1
fi

log_file="$1"
tail -f "$log_file" | grep --line-buffered -o '\[[0-9]\+/[0-9]\+\]' | while read -r line; do
    printf "\r%-9s" "$line"
done
