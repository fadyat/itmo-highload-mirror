#!/usr/bin/env sh

executable="$1"
shift
output=$("$executable" "$@" 2>/dev/null 0</dev/null)
exit_code=$?

echo "Command finished with code: $exit_code"
if [ -z "$output" ]; then
    exit
fi

while true; do
    printf "Display output in: (e/p/o/n/?) "
    read -r choice
    case $choice in
        p)
            echo "$output" | ${PAGER:-less}
            break
            ;;
        e)
            temp_file=$(mktemp)
            echo "$output" > "$temp_file"
            ${EDITOR:-vi} "$temp_file"
            rm "$temp_file"
            break
            ;;
        o)
            echo "$output"
            break
            ;;
        n)
            break
            ;;
        *)
            echo "e - open in editor"
            echo "p - open in pager"
            echo "o - print to stdout"
            echo "n - do nothing"
            ;;
    esac
done
