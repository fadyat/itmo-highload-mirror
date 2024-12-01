#!/usr/bin/env sh

if [ $# -eq 0 ]; then
    sed "s/[[:space:]]*$//"
else
    for file in "$@"; do
        if [ -d "$file" ]; then
            find "$file" -type f -exec sed -i 's/[[:space:]]*$//' {} \;
        else
            sed -i 's/[[:space:]]*$//' "$file"
        fi
    done
fi
