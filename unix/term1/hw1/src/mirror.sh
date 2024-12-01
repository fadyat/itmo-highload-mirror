#!/usr/bin/env sh

tmp_file=$(mktemp)
cat > "$tmp_file"

for arg in "$@"; do
    case "$arg" in
        H)
            rev "$tmp_file" > "$tmp_file.tmp" && mv "$tmp_file.tmp" "$tmp_file"
            ;;
        V)
            tac "$tmp_file" > "$tmp_file.tmp" && mv "$tmp_file.tmp" "$tmp_file"
            ;;
        *)
            ;;
    esac
done

cat "$tmp_file"
rm "$tmp_file"
