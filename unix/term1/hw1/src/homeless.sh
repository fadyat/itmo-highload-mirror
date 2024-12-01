#!/usr/bin/env sh

output=$(awk -F: '$6 == "/dev/null" { print $3 "\t" $1 }' /etc/passwd)
sorted_output=$(echo "$output" | sort -n -k1,1)
echo "$sorted_output"
printf "Total:\t%s\n" "$(echo "$output" | wc -l)"